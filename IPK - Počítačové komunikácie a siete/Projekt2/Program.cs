using System;
using System.CommandLine;
using System.CommandLine.Invocation;

namespace ipk_sniffer
{
    class Program
    {
        /*
         * Spracovanie argumentov pomocou knižnice System.Commandline
         */
        public static void Main(string[] args)
        {
            var interfaceOption = new Option<string>("-i") { Argument = new Argument<string> { Arity = ArgumentArity.ZeroOrOne } };
            var portOption = new Option<int>("-p") { Argument = new Argument<int> { Arity = ArgumentArity.ZeroOrOne } };
            var tcpOption = new Option<bool>("--tcp");
            var udpOption = new Option<bool>("--udp");
            var arpOption = new Option<bool>("--arp");
            var icmpOption = new Option<bool>("--icmp");
            var numbOption = new Option<int>("-n") { Argument = new Argument<int> { Arity = ArgumentArity.ZeroOrOne } };

            interfaceOption.AddAlias("--interface");
            tcpOption.AddAlias("-t");
            udpOption.AddAlias("-u");

            interfaceOption.Description = "rozhranie určené k naslúchaniu";
            portOption.Description = "filtrovanie paketov podľa portu, bez uvedenia portu sa akceptujú všetky porty";
            tcpOption.Description = "filter akceptuje len TCP pakety";
            udpOption.Description = "filter akceptuje len UDP pakety";
            arpOption.Description = "filter akceptuje len ARP rámce";
            icmpOption.Description = "filter akceptuje len ICMPv4 a ICMPv6 pakety";
            numbOption.Description = "počet paketov, ktoré sa majú na výstupe zobraziť";

            var command = new RootCommand
            {
                interfaceOption,
                portOption,
                tcpOption,
                udpOption,
                arpOption,
                icmpOption,
                numbOption
            };

            Sniffer inputSniffer = new Sniffer();

            command.Handler = CommandHandler.Create<string, int, bool, bool, bool, bool, int>(
                (string i, int p, bool tcp, bool udp, bool arp, bool icmp, int n) =>
                {
                    inputSniffer.Store_Arguments(i, p, tcp, udp, arp, icmp, n);
                });

            command.Invoke(args);
        }
    }
}
