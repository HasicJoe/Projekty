using System;
using System.CommandLine;
using System.CommandLine.Invocation;
using sniffer_params;


namespace ipk_sniffer
{
    class Sniffer
    {

        public static void Main(string[] args)
        {

            var interfaceOption = new Option<string>("-i") { Argument = new Argument<string> { Arity = ArgumentArity.ZeroOrOne }};
            var portOption = new Option<int>("-p") { Argument = new Argument<int> { Arity = ArgumentArity.ZeroOrOne}};
            var tcpOption = new Option<bool>("--tcp");
            var udpOption = new Option<bool>("--udp");
            var arpOption = new Option<bool>("--arp");
            var icmpOption = new Option<bool>("--icmp");
            var numbOption = new Option<int>("-n") { Argument = new Argument<int> {Arity = ArgumentArity.ZeroOrOne } };

            interfaceOption.AddAlias("--interface");
            tcpOption.AddAlias("-t");
            udpOption.AddAlias("-u");

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

            SnifferParams inputSnifferParams = new SnifferParams();

            command.Handler = CommandHandler.Create<string,int,bool,bool,bool,bool,int>(
                (string i, int p, bool tcp, bool udp, bool arp, bool icmp, int n) =>
                {
                    inputSnifferParams.Store_Arguments(i,p,tcp,udp,arp,icmp,n);
                });

            command.Invoke(args);
        }
    }
}