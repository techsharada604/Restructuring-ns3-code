/*
 * Copyright (c) 2026 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Sharada A Pandit <sharada.a.pandit@gmail.com>
 */

 // start-general-documentation
/*
 * Two LANs, each having three nodes and one switch, IPv4 addresses, Ping application.
 * /////////////////////////////////////////////////////////////////////////////////////////////////////////
 * //                                        Network Topology                                             //
 * //                                                                                                     //
 * //                                          10.1.0.0 /24                                               //
 * //                                          100Mbps, 1ms                                               //
 * //                                                                                                     //
 * //   n0    n1    n2 ------------------- n3 ------------------- n7 ------------------- n4   n5   n6     //
 * //    |     |     |                           100Mbps, 1ms                             |    |    |     //
 * //    |     |     |                        (Switch-to-Switch)                          |    |    |     //
 * //  ==================                    (CSMA Ethernet link)                       ==============    //
 * //      LAN 1 hosts                                                                    LAN 2 hosts     //
 * //                                                                                                     //
 * //                                                                                                     //
 * //             n3 = Switch 1                                            n7 = Switch 2                  //
 * //                                                                                                     //
 * /////////////////////////////////////////////////////////////////////////////////////////////////////////
 *
 * Five Ping packets are sent from n0 to n5, and the success/failure of these
 * packets is reported. The output will look like this:
 * PING 10.1.0.5 - 56 bytes of data; 84 bytes including ICMP and IPv4 headers.
 * 64 bytes from 10.1.0.5: icmp_seq=0 ttl=64 time=21.114 ms
 * 64 bytes from 10.1.0.5: icmp_seq=1 ttl=64 time=6.049 ms
 * 64 bytes from 10.1.0.5: icmp_seq=2 ttl=64 time=6.049 ms
 * 64 bytes from 10.1.0.5: icmp_seq=3 ttl=64 time=6.049 ms
 * 64 bytes from 10.1.0.5: icmp_seq=4 ttl=64 time=6.049 ms
 * 
 * --- 10.1.0.5 ping statistics ---
 * 5 packets transmitted, 5 received, 0% packet loss, time 4006ms
 * rtt min/avg/max/mdev = 6/9/21/6.708 ms
 */
 // end-general-documentation

#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/bridge-module.h"
#include "ns3/point-to-point-module.h"

//code body
using namespace ns3;

int
main(int argc, char* argv[])
{
    /*
     * Allows to parse command line arguments
     */
    CommandLine cmd;

    /*
     * senderNodeIndex and receiverNodeIndex will have a default value of 0(n0) & 5(n5) which will
     * be changed if argument is used.
     */
    int senderNodeIndex=0;
    int receiverNodeIndex=5;

    /*
     * The 'AddValue' API adds a program argument. It takes the name of
     * program-supplied argument, the help text displayed when program is run
     * with "--help" argument and a value that will reference the variable
     * whose value is to be set.
     *
     * Command line arguments can be invoked in the following way:
     * ./ns3 run "two-lans-connected-directly.cc --sender=0 --receiver=5"
     */

    cmd.AddValue("sender", "Index of the sender node", senderNodeIndex);
    cmd.AddValue("receiver", "Index of the receiver node", receiverNodeIndex);
    cmd.Usage("Tutorial example to demonstrate two LANs connected directly");
    cmd.Parse(argc, argv);

    // Create the 8 nodes n0, n1, n2... n7
    NodeContainer nodes;
    nodes.Create(8);

    // Add the CSMA LAN 1 nodes n0, n1, n2 to the container lan1
    NodeContainer lan1;
    lan1.Add(nodes.Get(0));
    lan1.Add(nodes.Get(1));
    lan1.Add(nodes.Get(2));

    // Get node n3 and designate it as Switch 1 (LAN 1 switch)
    Ptr<Node> lan1Switch = nodes.Get(3);

    // Add the CSMA LAN 2 nodes n4, n5, n6 to the container lan2
    NodeContainer lan2;
    lan2.Add(nodes.Get(4));
    lan2.Add(nodes.Get(5));
    lan2.Add(nodes.Get(6));

    // Get node n7 and designate it as Switch 2 (LAN 2 switch)
    Ptr<Node> lan2Switch = nodes.Get(7);

    /*
     * Create a CsmaHelper. The attribute DataRate is set to 100Mbps and
     * Delay is set to 1ms.
     */
    CsmaHelper csma;
    csma.SetChannelAttribute("Delay", StringValue("1ms"));
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));

    // Create NetDeviceContainers for the csma devices on LAN 1 and LAN 2.
    NetDeviceContainer lan1HostDevices;
    NetDeviceContainer lan2HostDevices;

    // Create NetDeviceContainers to store the switch ports for LAN 1 and LAN 2
    NetDeviceContainer lan1SwitchPorts;
    NetDeviceContainer lan2SwitchPorts;

    /*
     * Iterate through all nodes in LAN-1 and create a dedicated
     * CSMA link between each node and Switch 1.
     */
    for (uint32_t i = 0; i < lan1.GetN(); i++)
    {
        
        NodeContainer pair;
        pair.Add(lan1.Get(i));
        pair.Add(lan1Switch);

        NetDeviceContainer link = csma.Install(pair);

        lan1HostDevices.Add(link.Get(0));
        lan1SwitchPorts.Add(link.Get(1));
    }
    /*
     * Iterate through all nodes in LAN-2 and create a dedicated
     * CSMA link between each node and Switch 2.
     */
    for (uint32_t i = 0; i < lan2.GetN(); i++)
    {
        NodeContainer pair;
        pair.Add(lan2.Get(i));
        pair.Add(lan2Switch);

        NetDeviceContainer link = csma.Install(pair);

        lan2HostDevices.Add(link.Get(0));
        lan2SwitchPorts.Add(link.Get(1));
    }

    /*
     * Connect Switch 1 and Switch 2 using a CSMA link.
     */
    NodeContainer switchPair;
    switchPair.Add(lan1Switch);
    switchPair.Add(lan2Switch);

    /*
    * Create NetDeviceContainer for the CSMA link between Switch 1 and Switch 2.
    * The first device (index 0) belongs to lan1Switch,
    * and the second device (index 1) belongs to lan2Switch.
    */
    NetDeviceContainer interSwitchLink = csma.Install(switchPair);

    /*
    * Add the corresponding NetDevices to each switch's port list.
    * These will act as inter-switch ports when the bridge is installed.
    */
    lan1SwitchPorts.Add(interSwitchLink.Get(0));
    lan2SwitchPorts.Add(interSwitchLink.Get(1));

    /*
     * Install BridgeNetDevice on the switches to enable Layer-2 forwarding.
     */
    BridgeHelper bridge;

    bridge.Install(lan1Switch, lan1SwitchPorts);
    bridge.Install(lan2Switch, lan2SwitchPorts);

    // Install the internet stack on all the host nodes
    InternetStackHelper stack;
    stack.Install(lan1);
    stack.Install(lan2);

    /*
     * Assign IPv4 addresses to all the interfaces.
     * This example has 1 network:
     *      - LAN consisting of all hosts with network id 10.1.0.0/24
     */
    Ipv4AddressHelper address;
    address.SetBase("10.1.0.0", "255.255.255.0");

    Ipv4InterfaceContainer lan1Interfaces =
        address.Assign(lan1HostDevices);

    Ipv4InterfaceContainer lan2Interfaces =
        address.Assign(lan2HostDevices);

    /*
     * Ping from node with index senderNodeIndex to node with index receiverNodeIndex
     * Get the first Address of the first Interface (not including loopback) of both the nodes
     * GetAddress takes in the index of the interface and index of ip address as argument
     * Since Loopback is index 0 we use 1 for interface and 0 for address
     */
    Address destination = nodes.Get(receiverNodeIndex)
        ->GetObject<Ipv4>()
        ->GetAddress(1, 0)
        .GetLocal();

    Address source = nodes.Get(senderNodeIndex)
        ->GetObject<Ipv4>()
        ->GetAddress(1, 0)
        .GetLocal();

    /*
     * Create a ping application with receiverNodeIndex as destination and senderNodeIndex as
     * source.
     */
    PingHelper pingHelper(destination, source);
    pingHelper.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    pingHelper.SetAttribute("Size", UintegerValue(56));
    pingHelper.SetAttribute("Count", UintegerValue(5));

    // Install the ping application on node n3.
    ApplicationContainer apps =
        pingHelper.Install(nodes.Get(senderNodeIndex));
    apps.Start(Seconds(1));
    apps.Stop(Seconds(6));

    /*
     * Set the stop time for the simulator. Run the simulation, and invoke
     * 'Destroy' at the end of the simulation.
     */
    Simulator::Stop(Seconds(6.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
