import java.io.*;
import java.lang.*;
import java.net.*;



class ControllerThread  implements Runnable{

    private Router router;
    private PrintWriter out;
    private BufferedReader in;
    private Thread thread;
    private Socket s;
    public ControllerThread(Socket s, Router router)
    {
	try{
	    in = new BufferedReader(new InputStreamReader(s.getInputStream()));
	    out = new PrintWriter(s.getOutputStream(),true);
	    this.s = s;
	    this.router = router;
	    thread = new Thread(this);
	    thread.start();
	}catch(Exception e){}
    }

    public void run()
    {
	while(true)
	    {
		try{
		    out.println("poll*");
		    String cmd = in.readLine();
		    int ind1, ind2;
		    String op, subop;
		    //List<RouterInfo> routersBis = new ArrayList();
		    ind1 = cmd.indexOf(' ');
		    op = cmd.substring(0, ind1);
		    if(op.equalsIgnoreCase("neighborhood"))
			{
			    ind2 = cmd.indexOf(' ', ind1+1);
			    if(ind2 == -1)
				ind2 = cmd.indexOf('*');
			    
			    subop = cmd.substring(ind1+1, ind2);
			    
			    if(subop.equalsIgnoreCase("ok"))
				{
				    // TODO : Okay =D
				    System.out.println("Ok");
				}
			    else if(subop.equalsIgnoreCase("newlist"))
				{
				    router.delRouters();
				    ind1 = cmd.indexOf('[');
				    ind2 = cmd.indexOf(']');
				    String nData[] = cmd.substring(ind1+1, ind2).split("; ");
				    int nbNeighbor = nData.length;
				    
				    
				    if(ind1 != ind2 - 1){
					for(int i = 0 ; i < nbNeighbor ; i++)
					    {
						RouterInfo routerInfos = new RouterInfo();
						String neighborInfo[] = nData[i].split(",");
						routerInfos.addid(neighborInfo[0]);
						routerInfos.addCost(Integer.parseInt(neighborInfo[1]));
						
						neighborInfo = neighborInfo[2].split(":");
						routerInfos.addPort(Integer.parseInt(neighborInfo[1]));
						String[] ipString = neighborInfo[0].split("\\.");
						byte ipByte[] = new byte[4];
						for(int j = 0 ; j < 4 ; j++)
						    {
							int val = Integer.parseInt(ipString[j]);
							if(val < 0 || val > 255)
							    throw new Exception("that's not a valid IP address");
							
							ipByte[j] = (byte)val;
						    }
						routerInfos.addIP(InetAddress.getByAddress(ipByte));
						router.addRouter(routerInfos);
						//routersBis.add(routerInfos);
						//infos[i].s = new Socket(InetAddress.getByAddress(ipByte), Integer.parseInt(neighborInfo[1]));
					    }
				    }
				    /*for(RouterInfo ri: router.routers){
					if(!router.routersBis.contains(ri))
					    router.delRouter(ri.getId());
				    }
				    
				    for(RouterInfo ri: routersBis){
					if(!router.routers.contains(ri))
					    router.delRouter(ri.getId());
					    }*/
				    router.connect();
				    router.initRouteTable();
				    //router.afficherRouteTable();
				    thread.sleep(router.controllerUpdateInterval*1000);
				    
				}
			    else
				throw new Exception("Vector invalide");
			}
		}
		catch(Exception e){
		    System.err.println("Parsing failure");
		    e.printStackTrace();
		}
	    }
	
    }
}


