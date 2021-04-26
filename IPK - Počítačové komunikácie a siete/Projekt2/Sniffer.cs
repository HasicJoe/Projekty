using System;
using SharpPcap;
using PacketDotNet;

namespace ipk_sniffer
{
    class Sniffer
    {
        public string Name { get; set; }
        public int? Port { get; set; }
        public bool TcpFilter { get; set; }
        public bool UdpFilter { get; set; }
        public bool ArpFilter { get; set; }
        public bool IcmpFilter { get; set; }
        public int? NumberOfPackets { get; set; }
        public ICaptureDevice device { get; set; }


        /*
         *  Ukladanie spracovaných argumentov do objektu pre jednoduchšiu manipuláciu
         */
        public void Store_Arguments(string interface_name, int input_port, bool tcp, bool udp, bool arp, bool icmp, int n)
        {
            Sniffer inputSniffer = new Sniffer
            {
                Name = interface_name,
                Port = input_port,
                TcpFilter = tcp,
                UdpFilter = udp,
                ArpFilter = arp,
                IcmpFilter = icmp,
                NumberOfPackets = n
            };
            Decide_Sniffing(inputSniffer);
        }


        /*
         *  Kontrola správnosti argumentov -p a -n
         */
        public void Check_Arguments(Sniffer inputParameters)
        {
            if(inputParameters.NumberOfPackets <= 0)
            {
                inputParameters.NumberOfPackets = 1;
            }

            if (inputParameters.Port < 0 || inputParameters.Port > 65535)
            {
                Console.WriteLine($"Nesprávny port {inputParameters.Port}" +
                                  $", použite port v rozmedzí <0, 65535>");
                Environment.Exit(1);
            }
        }

        /*
         *  Riadenie činnosti sieťového analzyátoru
         */
        public void Decide_Sniffing(Sniffer inputSniffer)
        {
            // kontrola správnosti číselných argumentov
            Check_Arguments(inputSniffer);

            // výpis aktívnych rozhraní
            if (inputSniffer.Name == null)
            {
                Write_Active_Devices(0);
            }
            else
            {
                inputSniffer.device = Find_Device(inputSniffer.Name);

                if (inputSniffer.device == null)
                {
                    Console.WriteLine($"Zadané rozhranie {inputSniffer.Name} sa nenašlo!");
                    Console.WriteLine("Môžeťe použiť následovné rozhrania:");
                    Write_Active_Devices(1);
                }

                try
                {
                    int readTimeoutMilliseconds = 500;
                    inputSniffer.device.Open(DeviceMode.Promiscuous, readTimeoutMilliseconds);
                }
                catch (PcapException)
                {
                    Console.WriteLine($"Nepodarilo sa spojit zo zadaným rozhraním {inputSniffer.Name}");
                    Environment.Exit(1);
                }

                // v prípade, že neboli špecifikované filtre pre protokoly, nastavíme všetky za povolené
                if ((!inputSniffer.ArpFilter) && (!inputSniffer.IcmpFilter) &&
                    (!inputSniffer.TcpFilter) && (!inputSniffer.UdpFilter))
                {
                    inputSniffer.ArpFilter = true;
                    inputSniffer.UdpFilter = true;
                    inputSniffer.IcmpFilter = true;
                    inputSniffer.ArpFilter = true;
                }
                Start_Sniffing(inputSniffer);
            }
        }


        /*
         *  Výpis hlavičiek pre TCP a UDP pakety
         */
        public void Write_Header(DateTime packetTime, string sourceIp, ushort sourcePort, string destIp, ushort destPort, string offset, int len)
        {
            string monthsDays = packetTime.ToString("MM-dd");
            string hoursMinutesEtc = packetTime.ToString("HH:mm:ss");
            Console.WriteLine(
                $"{packetTime.Year}-{monthsDays}T{hoursMinutesEtc}." +
                $"{packetTime.Millisecond}{offset} {sourceIp} : {sourcePort} > {destIp} : {destPort}, length {len} bytes");
        }



        /*
         *  Výpis hlavičiek pre ICMPv4 a ICMPv6 pakety(chýbajú porty)
         */
        public void Write_Header_No_Port(DateTime packetTime, string sourceIp, string destIp, string offset, int len)
        {
            string monthsDays = packetTime.ToString("MM-dd");
            string hoursMinutesEtc = packetTime.ToString("HH:mm:ss");
            Console.WriteLine(
                $"{packetTime.Year}-{monthsDays}T{hoursMinutesEtc}." +
                $"{packetTime.Millisecond}{offset} {sourceIp} > {destIp}, length {len} bytes");
        }


        /*
         *  Výpočet časového posunu aby výsledný formát RFC3339
         */
        public string Get_Time_Offset(DateTime time)
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
                if (timeOffset.Hours == 0)
                {
                    return "Z";
                }
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


        /*
         *  Výpis paketu po riadkoch
         */
        public void Write_Data_line(byte[] line, int offset, int addPadding)
        {
            // výpočet offsetu
            string line_offset = "0x" + offset.ToString("X3") + "0: ";
            Console.Write(line_offset);

            for (int i = 0; i < line.Length; i++)
            {
                // výpis bytov v 16tkovej sústave pri hodnote < 16 pridáme "0" aby sme zachovali formát
                int value = Convert.ToInt32(line[i]);

                if (value < 16)
                {
                    Console.Write("0" + Convert.ToString(value, 16) + " ");
                }
                else
                {
                    Console.Write(Convert.ToString(value, 16) + " ");
                }
                // medzera rozdelujuca 8 - 8 bytov
                if(i == 7)
                {
                    Console.Write(" ");
                }
            }

            // v prípade že ide o posledný riadok zarovnáme výpis medzerami
            if (addPadding != 0)
            {
                for (int i = 0; i < addPadding; i++)
                {
                    Console.Write(" ");
                }
            }

            // výpis tisknutelných znakov pri výbere som vychádzal z: http://facweb.cs.depaul.edu/sjost/it212/documents/ascii-pr.htm
            for (int i = 0; i < line.Length; i++)
            {
                int asciiValue = Convert.ToInt32(line[i]);

                if (asciiValue > 31 && asciiValue < 128)
                {
                    Console.Write((char)asciiValue);
                }
                else
                {
                    Console.Write(".");
                }
                // rozdelenie 8 - 8 bytov
                if(i == 7)
                {
                    Console.Write(" ");
                }
            }
            Console.Write("\n");
        }


        /*
         *  Rozdelenie dát paketu po riadkoch(1 riadok = 16 bytov)
         *  a následne odošle riadok funkcii pre výpis
         */
        public void Save_Lines(byte[] packetData)
        {
            int i = 0;
            for (; (i + 1) * 16 < packetData.Length; i++)
            {
                byte[] line = new byte[16];
                for (int j = 0; j < 16; j++)
                {
                    line[j] = packetData[(i * 16) + j];
                }
                Write_Data_line(line, i, 0);
            }

            // spracovanie posledného riadku dat - zvyčajne menej ako 16 bytov
            int lastLineLen = packetData.Length % 16;
            // hraničná hodnota presne 16 bytov na poslednom riadku
            if (lastLineLen == 0)
            {
                lastLineLen = 16;
            }
            byte[] lastLine = new byte[lastLineLen];

            for (int j = 0; j < lastLineLen; j++)
            {
                lastLine[j] = packetData[(i * 16) + j];
            }
            // vypočítame padding aby sme mali výpis zarovnaný
            int padding = ((16 - lastLineLen) * 3);
            // pridáme pre kratšie riadky padding pre rozdelenie 8 8 
            if (lastLineLen < 8)
            {
                padding++;
            }
            Write_Data_line(lastLine, i, padding);
        }

        
        /*
         *  Zachytávanie paketov podľa filtrov špecifikovaných argumentami
         */
        public void Start_Sniffing(Sniffer inputSniffer)
        {
            // získavame pakety pokiaľ nesplníme počet určený uživateľom (-n argument)
            while (inputSniffer.NumberOfPackets > 0)
            {
                RawCapture rawPacket = inputSniffer.device.GetNextPacket();

                if(rawPacket != null)
                {
                    if (rawPacket.LinkLayerType == LinkLayers.Ethernet)
                    {

                        Packet packet = Packet.ParsePacket(LinkLayers.Ethernet, rawPacket.Data);
                        // podporované pakety tcp,udp,arp,icmpv4,icmpv6
                        ArpPacket arpPacket = packet.Extract<PacketDotNet.ArpPacket>();
                        IcmpV6Packet icmpV6Packet = packet.Extract<PacketDotNet.IcmpV6Packet>();
                        IcmpV4Packet icmpV4Packet = packet.Extract<PacketDotNet.IcmpV4Packet>();
                        TcpPacket tcpPacket = packet.Extract<PacketDotNet.TcpPacket>();
                        UdpPacket udpPacket = packet.Extract<PacketDotNet.UdpPacket>();
                       
                        DateTime time = rawPacket.Timeval.Date;
                        // spočítame časový offset aby bol zápis podľa formátu RFC3339
                        string offset = Get_Time_Offset(time);

                        // ak ide o arp packet a máme povolené spracovanie spracujeme ho, parameter -p pri spracovaní arp paketov ignorujeme
                        if (arpPacket != null && inputSniffer.ArpFilter)
                        {
                            string senderIp = arpPacket.SenderProtocolAddress.ToString();
                            string targetIp = arpPacket.TargetProtocolAddress.ToString();
                            Write_Header_No_Port(time, senderIp, targetIp, offset, rawPacket.Data.Length);
                            Save_Lines(rawPacket.Data);
                            inputSniffer.NumberOfPackets = inputSniffer.NumberOfPackets - 1;
                        }
                        // ak ide o tcp packet a máme povolené spracovanie spracujeme ho
                        else if (tcpPacket != null && inputSniffer.TcpFilter)
                        {
                            string sourceIp = "";
                            string destIp = "";

                            if (tcpPacket.ParentPacket is IPv4Packet)
                            {
                                sourceIp = packet.Extract<PacketDotNet.IPv4Packet>().SourceAddress.ToString();
                                destIp = packet.Extract<PacketDotNet.IPv4Packet>().DestinationAddress.ToString();
                               
                            }
                            else if (tcpPacket.ParentPacket is IPv6Packet)
                            {
                                 sourceIp = packet.Extract<PacketDotNet.IPv6Packet>().SourceAddress.ToString();
                                 destIp = packet.Extract<PacketDotNet.IPv6Packet>().DestinationAddress.ToString();

                            }

                            if (sourceIp == String.Empty || destIp == String.Empty)
                            {
                                Console.WriteLine("Nepodarilo sa nájsť zdrojovú/cielovú Ip adresu TCP paketu");
                                Environment.Exit(1);
                            }

                            if (inputSniffer.Port != 0)
                            {
                                if ((tcpPacket.DestinationPort == inputSniffer.Port) ||
                                    (tcpPacket.SourcePort == inputSniffer.Port))
                                {
                                    Write_Header(time, sourceIp, tcpPacket.SourcePort, destIp, tcpPacket.DestinationPort, offset, rawPacket.Data.Length);
                                    Save_Lines(rawPacket.Data);
                                    inputSniffer.NumberOfPackets = inputSniffer.NumberOfPackets - 1;
                                }
                            }
                            else
                            {
                                Write_Header(time, sourceIp, tcpPacket.SourcePort, destIp, tcpPacket.DestinationPort, offset, rawPacket.Data.Length);
                                Save_Lines(rawPacket.Data);
                                inputSniffer.NumberOfPackets = inputSniffer.NumberOfPackets - 1;
                            }
                        }
                        // ak ide o udp packet a máme povolené spracovanie spracujeme ho
                        else if (udpPacket != null && inputSniffer.UdpFilter)
                        {
                            string sourceIp = "";
                            string destIp = "";

                            if (udpPacket.ParentPacket is IPv4Packet)
                            {
                                sourceIp = packet.Extract<PacketDotNet.IPv4Packet>().SourceAddress.ToString();
                                destIp = packet.Extract<PacketDotNet.IPv4Packet>().DestinationAddress.ToString();

                            }
                            else if (udpPacket.ParentPacket is IPv6Packet)
                            {
                                sourceIp = packet.Extract<PacketDotNet.IPv6Packet>().SourceAddress.ToString();
                                destIp = packet.Extract<PacketDotNet.IPv6Packet>().DestinationAddress.ToString();
                            }

                            if (sourceIp == String.Empty || destIp == String.Empty)
                            {
                                Console.WriteLine("Nepodarilo sa nájsť zdrojovú/cielovú Ip adresu UDP paketu");
                                Environment.Exit(1);
                            }
                            
                            if (inputSniffer.Port != 0)
                            {
                                if ((udpPacket.DestinationPort == inputSniffer.Port) ||
                                    (udpPacket.SourcePort == inputSniffer.Port))
                                {
                                    Write_Header(time, sourceIp, udpPacket.SourcePort, destIp, udpPacket.DestinationPort, offset, rawPacket.Data.Length);
                                    Save_Lines(rawPacket.Data);
                                    inputSniffer.NumberOfPackets = inputSniffer.NumberOfPackets - 1;
                                }
                            }
                            else
                            {
                                Write_Header(time, sourceIp, udpPacket.SourcePort, destIp, udpPacket.DestinationPort, offset, rawPacket.Data.Length);
                                Save_Lines(rawPacket.Data);
                                inputSniffer.NumberOfPackets = inputSniffer.NumberOfPackets - 1;
                            }
                        }
                        // ak ide o icmpv4 packet a máme povolené spracovanie spracujeme ho, pri icmp paketoch ignorujeme parameter -p
                        else if (icmpV4Packet != null && inputSniffer.IcmpFilter)
                        {
                            string sourceIp = packet.Extract<PacketDotNet.IPv4Packet>().SourceAddress.ToString();
                            string destIp = packet.Extract<PacketDotNet.IPv4Packet>().DestinationAddress.ToString();

                            if (destIp == null || sourceIp == null)
                            {
                                Console.WriteLine("Nepodarilo sa spracovať adresy ICMPv4 Paketu");
                                Environment.Exit(0);
                            }

                            Write_Header_No_Port(time, sourceIp, destIp, offset, rawPacket.Data.Length);
                            Save_Lines(rawPacket.Data);
                            inputSniffer.NumberOfPackets = inputSniffer.NumberOfPackets - 1;
                        }
                        else if (icmpV6Packet != null && inputSniffer.IcmpFilter)
                        {

                            string sourceIp = packet.Extract<PacketDotNet.IPv6Packet>().SourceAddress.ToString();
                            string destIp = packet.Extract<PacketDotNet.IPv6Packet>().DestinationAddress.ToString();

                            if (sourceIp == null || destIp == null)
                            {
                                Console.WriteLine("Nepodarilo sa spracovať adresy ICMPv6 paketu");
                                Environment.Exit(1);
                            }
                            Write_Header_No_Port(time, sourceIp, destIp, offset, rawPacket.Data.Length);
                            Save_Lines(rawPacket.Data);
                            inputSniffer.NumberOfPackets = inputSniffer.NumberOfPackets - 1;
                        }
                    }
                }
            }
            inputSniffer.device.Close();
            Environment.Exit(0);
        }


        /*
         *  Metóda hľadá vhodné zariadenie k počúvaniu - (prepínač -i/--interface rozhranie)
         */
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


        /*
         * Metóda vypíše zoznam aktívnych zariadení a ukončí činnosť programu
         */
        public void Write_Active_Devices(int exitCode)
        {
            var devices = CaptureDeviceList.Instance;

            foreach (ICaptureDevice device in devices)
            {
                Console.WriteLine($"{device.Name}");
            }
            Environment.Exit(exitCode);
        }
    }
}
