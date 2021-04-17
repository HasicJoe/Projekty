using System;
using SharpPcap;
using PacketDotNet;


namespace sniffer_params
{
    class SnifferParams
    {
        public string Name { get; set; }
        public int? Port { get; set; }
        public bool TcpFilter { get; set; }
        public bool UdpFilter { get; set; }
        public bool ArpFilter { get; set; }
        public bool IcmpFilter { get; set; }
        public int? NumberOfPackets { get; set; }
        public ICaptureDevice device { get; set; }


        public void Store_Arguments(string interface_name,int input_port, bool tcp, bool udp, bool arp, bool icmp, int n)
        {
            // vytvoríme novú inštanciu
            
            SnifferParams inputSnifferParams = new SnifferParams
            {
                Name = interface_name,
                Port = input_port,
                TcpFilter = tcp,
                UdpFilter = udp,
                ArpFilter = arp,
                IcmpFilter = icmp,
                NumberOfPackets = n
            };
            Decide_Sniffing(inputSnifferParams);
        }


        public void Decide_Sniffing(SnifferParams inputSnifferParams)
        {
            if (inputSnifferParams.NumberOfPackets == 0)
            {
                inputSnifferParams.NumberOfPackets = 1;
            }

            if (inputSnifferParams.Name == null)
            {
                Write_Active_Devices();
            }
            else
            {
                inputSnifferParams.device = Find_Device(inputSnifferParams.Name);

                if (inputSnifferParams.device == null)
                {
                    Console.WriteLine($"Zadané rozhranie {inputSnifferParams.Name} sa nenašlo!");
                    Environment.Exit(1);
                }

                //https://www.codeproject.com/Articles/12458/SharpPcap-A-Packet-Capture-Framework-for-NET#statistics
                Start_Sniffing(inputSnifferParams);

            }
        }


        public void Write_Header(DateTime packetTime,string sourceIp, ushort sourcePort,string destIp,ushort destPort, string offset,int len)
        {
            Console.WriteLine(
                $"{packetTime.Year}-{packetTime.Month}-{packetTime.Day}T{packetTime.Hour}:{packetTime.Minute}:{packetTime.Second}." +
                $"{packetTime.Millisecond}{offset} {sourceIp} : {sourcePort} > {destIp} : {destPort}, length {len} bytes");
        }

        public void Write_Arp_Header(DateTime packetTime, string offset, int len)
        {
            Console.WriteLine($"{packetTime.Year}-{packetTime.Month}-{packetTime.Day}T{packetTime.Hour}:" +
                              $"{packetTime.Minute}:{packetTime.Second}{packetTime.Millisecond}{offset}, length {len} bytes");
        }

        public void Write_Icmp_Header(DateTime packetTime, string sourceIp, string destIp, string offset, int len)
        {
            Console.WriteLine(
                $"{packetTime.Year}-{packetTime.Month}-{packetTime.Day}T{packetTime.Hour}:{packetTime.Minute}:" +
                $"{packetTime.Second}{packetTime.Millisecond}{offset} {sourceIp} > {destIp}, length {len} bytes");
        }


        public string Get_Offset_To_Print_Header(DateTime time)
        {
            TimeSpan timeOffset = TimeZoneInfo.Local.GetUtcOffset(time);
            string stringOffset = "";
            string hoursOffset = "";
            string minutesOffset = "";

            if (timeOffset.Hours < 0)
            {
                stringOffset += "-";
            }
            else
            {
                stringOffset += "+";
            }

            if (timeOffset.Hours < 10)
            {
                hoursOffset += "0";
            }

            if (timeOffset.Minutes < 10)
            {
                minutesOffset += "0";
            }
            return stringOffset + hoursOffset + timeOffset.Hours.ToString() + ":" + minutesOffset + timeOffset.Minutes.ToString();
        }


        public void Write_Data_line(byte[] line, int offset,int addPadding)
        {
            string line_offset = "0x" + offset.ToString("X3") + "0: ";
            Console.Write(line_offset);

            for (int i = 0; i < line.Length; i++)
            {
                int value = Convert.ToInt32(line[i]);

                if (value < 16)
                {
                    Console.Write("0" + Convert.ToString(line[i], 16) + " ");
                }
                else
                {
                    Console.Write(Convert.ToString(line[i], 16) + " ");
                }
            }

            if (addPadding != 0)
            {
                for (int i = 0; i < addPadding; i++)
                {
                    Console.Write(" ");
                }
            }

            for (int i = 0; i < line.Length; i++)
            {
                int asciiValue = Convert.ToInt32(line[i]);
                // http://facweb.cs.depaul.edu/sjost/it212/documents/ascii-pr.htm
                if (asciiValue > 31 && asciiValue < 128)
                {
                    Console.Write((char)asciiValue);
                }
                else
                {
                    Console.Write(".");
                }
            }
            Console.Write("\n");
        }


        public void Save_Lines(byte[] packetData)
        {
            int i = 0;
            for (; (i+1) * 16 < packetData.Length; i++)
            {
                byte[] line = new byte[16];
                for (int j = 0; j < 16; j++)
                {
                    line[j] = packetData[(i * 16) + j];
                }
                Write_Data_line(line, i,0);
            }

            int lastLineLen = packetData.Length % 16;
            byte[] lastLine = new byte[lastLineLen];

            for (int j = 0; j < lastLineLen; j++)
            {
                lastLine[j] = packetData[(i * 16) + j];
            }
            int padding = ((16 - lastLineLen) * 3);
            Write_Data_line(lastLine, i, padding);
        }


        public void Start_Sniffing(SnifferParams inputSnifferParams)
        {
            if ((!inputSnifferParams.ArpFilter) && (!inputSnifferParams.IcmpFilter) && (!inputSnifferParams.TcpFilter) &&
                (!inputSnifferParams.UdpFilter))
            {
                inputSnifferParams.ArpFilter = true;
                inputSnifferParams.UdpFilter = true;
                inputSnifferParams.IcmpFilter = true;
                inputSnifferParams.ArpFilter = true;
            }

            int readTimeoutMilliseconds = 1000;
            inputSnifferParams.device.Open(DeviceMode.Promiscuous, readTimeoutMilliseconds);
            RawCapture rawPacket = null;
            
            while ((rawPacket = inputSnifferParams.device.GetNextPacket()) != null && inputSnifferParams.NumberOfPackets > 0)
            {
                var packet = Packet.ParsePacket(rawPacket.LinkLayerType, rawPacket.Data);

                if (packet is EthernetPacket)
                {
                    var arpPacket = packet.Extract<PacketDotNet.ArpPacket>();
                    var icmpV6Packet = packet.Extract<PacketDotNet.IcmpV6Packet>();
                    var icmpV4Packet = packet.Extract<PacketDotNet.IcmpV4Packet>();
                    var tcpPacket = packet.Extract<PacketDotNet.TcpPacket>();
                    var udpPacket = packet.Extract<PacketDotNet.UdpPacket>();

                    DateTime time = rawPacket.Timeval.Date;
                    string offset = Get_Offset_To_Print_Header(time);

                    if (arpPacket != null && inputSnifferParams.ArpFilter)
                    {
                        byte[] packetData = rawPacket.Data;
                        int length = rawPacket.Data.Length;
                        Write_Arp_Header(time, offset, length);
                        Save_Lines(packetData);
                        inputSnifferParams.NumberOfPackets = inputSnifferParams.NumberOfPackets - 1;
                    }
                    else if (tcpPacket != null && inputSnifferParams.TcpFilter)
                    {
                        var ip = packet.Extract<PacketDotNet.IPPacket>();
                        string sourceIp = ip.SourceAddress.ToString();
                        string destIp = ip.DestinationAddress.ToString();
                        byte[] packetData = rawPacket.Data;
                        int length = rawPacket.Data.Length;

                        if (inputSnifferParams.Port != 0)
                        {
                            if ((tcpPacket.DestinationPort == inputSnifferParams.Port) ||
                                (tcpPacket.SourcePort == inputSnifferParams.Port))
                            {
                                Write_Header(time, sourceIp,tcpPacket.SourcePort,destIp,tcpPacket.DestinationPort, offset, length);
                                Save_Lines(packetData);
                                inputSnifferParams.NumberOfPackets = inputSnifferParams.NumberOfPackets - 1;
                            }
                        }
                        else
                        {
                            Write_Header(time, sourceIp, tcpPacket.SourcePort, destIp, tcpPacket.DestinationPort, offset,length);
                            Save_Lines(packetData);
                            inputSnifferParams.NumberOfPackets = inputSnifferParams.NumberOfPackets - 1;
                        }
                    }
                    else if (udpPacket != null && inputSnifferParams.UdpFilter)
                    {
                        var ip = packet.Extract<PacketDotNet.IPPacket>();
                        string sourceIp = ip.SourceAddress.ToString();
                        string destIp = ip.DestinationAddress.ToString();
                        byte[] packetData = rawPacket.Data;
                        int length = rawPacket.Data.Length;

                        if (inputSnifferParams.Port != 0)
                        {
                            if ((udpPacket.DestinationPort == inputSnifferParams.Port) ||
                                (udpPacket.SourcePort == inputSnifferParams.Port))
                            {
                                Write_Header(time,sourceIp,udpPacket.SourcePort,destIp,udpPacket.DestinationPort,offset,length);
                                Save_Lines(packetData);
                                inputSnifferParams.NumberOfPackets = inputSnifferParams.NumberOfPackets - 1;
                            }
                        }
                        else
                        {
                            Write_Header(time, sourceIp, udpPacket.SourcePort, destIp, udpPacket.DestinationPort, offset, length);
                            Save_Lines(packetData);
                            inputSnifferParams.NumberOfPackets = inputSnifferParams.NumberOfPackets - 1;
                        }
                    }
                    else if (icmpV4Packet != null && inputSnifferParams.IcmpFilter)
                    {
                        var ip = packet.Extract<PacketDotNet.IPPacket>();
                        string sourceIp = ip.SourceAddress.ToString();
                        string destIp = ip.DestinationAddress.ToString();
                        byte[] packetData = rawPacket.Data;
                        int length = rawPacket.Data.Length;
                        Write_Icmp_Header(time, sourceIp, destIp, offset, length);
                        Save_Lines(packetData);
                        inputSnifferParams.NumberOfPackets = inputSnifferParams.NumberOfPackets - 1;
                    }
                    else if (icmpV6Packet != null && inputSnifferParams.IcmpFilter)
                    {
                        var ip = packet.Extract<PacketDotNet.IPPacket>();
                        string sourceIp = ip.SourceAddress.ToString();
                        string destIp = ip.DestinationAddress.ToString();
                        byte[] packetData = rawPacket.Data;
                        int length = rawPacket.Data.Length;
                        Write_Icmp_Header(time,sourceIp,destIp, offset,length);
                        Save_Lines(packetData);
                        inputSnifferParams.NumberOfPackets = inputSnifferParams.NumberOfPackets - 1;
                    }
                }
            }
            Environment.Exit(0);
        }


        public ICaptureDevice Find_Device(string inputDeviceName)
        {
            var devices = CaptureDeviceList.Instance;

            foreach (ICaptureDevice device in devices)
            {
                if (device.Name == inputDeviceName)
                {
                    return device;
                }
            }
            return null;
        }


        public void Write_Active_Devices()
        {
            var devices = CaptureDeviceList.Instance;
    
            foreach (ICaptureDevice device in devices)
            {
                Console.WriteLine($"{device.Name}, {device.Description}");
            }
            Environment.Exit(0);
        }
    }
}
