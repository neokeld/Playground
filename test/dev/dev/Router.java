/*to Do
 *tester readMessage
 *ajouter ping
 **/
import java.net.*;
import java.io.*;
import java.util.*;
import java.net.*;
import java.text.SimpleDateFormat;
import java.nio.ByteBuffer;



/** Classe principale du routeur**/
public class Router {

    final static int SERVER = 0;
    final static int CLIENT = 1;
    public PrintWriter controlerOut; 
    private BufferedReader controlerIn; 
    private int nbRouters=0; 
    private List<RouterInfo> alreadyConnectedTo;
    private List<RouterInfo> routers;
    public List<Integer> listId;
    public List<String> alreadyConnected;
    public String id;
    public int port;
    public RouterInfo controllerInfo;
    private int controllerPort;
    private InetAddress controllerAd;
    public int controllerUpdateInterval;
    public int routerUpdateInterval;
    public int ttlValue;
    public int defaultPingPacketCount;
    public int defaultPacketTimeoutValue;
    public int defaultDVTimeoutValue;


    private List<String> idDes;
    private List<String> idNext;
    private List<Integer> cost;


    private void launch(String id, Router router){
	this.id = id;
	controllerInfo = new RouterInfo();
	router.parseConfigFile("testRouter.cfg");
	listId = new ArrayList<Integer>();
	idDes = new ArrayList<String>();
	idNext = new ArrayList<String>();
	cost = new ArrayList<Integer>();
	alreadyConnected = new ArrayList<String>();
	alreadyConnected.add(this.id);
	routers = new ArrayList();
	alreadyConnectedTo = new ArrayList();	
	try{
	  
	    ServerSocket ss = new ServerSocket(port);
	    Socket controlerSocket = new Socket(/*controllerAd*/ "localhost" ,controllerPort);
	    controllerInfo.addSocket(controlerSocket);
	    controlerIn = new BufferedReader(new InputStreamReader(controlerSocket.getInputStream()));
	    controlerOut = new PrintWriter(controlerSocket.getOutputStream(),true);
	    routerControler(controlerOut,controlerIn,id,port);
	    new ControllerThread(controlerSocket,router);
	    new Commandes(router); 
	    //new PeriodicVector(router,routerUpdateInterval);
	    printWelcome(port,this.id);
	    while (true){
		new RouterThread(ss.accept(),router,Router.SERVER,new RouterInfo());
	    }
	}catch(Exception e){ e.printStackTrace();}
    }
    
     private void routerControler(PrintWriter out, BufferedReader in, String id, int port){
	
	 try{
	     out.println("log in as "+id+" port "+port+"*");
	     String message = in.readLine();
	     String split[] = message.split(" ");
	     if(split[0].equalsIgnoreCase("greeting")){
		 this.id = split[1].substring(0,split[1].length()-1); 
	     }
	 }catch(Exception e){ e.printStackTrace();}
	 
     }


    public static void main(String args[]){
	try{	
	    Router router = new Router(); 
	    int port;
	    String id;
	    //port = 1800; 
	    id = args[0];
	    router.launch(id,router);  
	}
	catch (Exception e) { }
    }
    
    
    private void printWelcome(int port,String id)
    {
	System.out.println("--------");
	System.out.println("Serveur : Meet the project");
	System.out.println("--------");
	System.out.println("Demarre sur le port : "+port);
	System.out.println("--------");
	System.out.println("ID : "+id);
	System.out.println("--------");
	System.out.println("Quitter : tapez \"quit\"");
	System.out.println("Nombre de connectes : tapez \"total\"");
	System.out.println("--------");
    }
    
    
    
    synchronized public void sendAll(String message)
    {
	PrintWriter out; 
	if(!routers.isEmpty()){
	    for (int i = 0; i < routers.size(); i++){
		if(!routers.get(i).getId().equals(this.id)){
		    sendMessage(message,routers.get(i).getId());
		}
	    }
	}
    }
    
    
    public void sendMessage(String message,String id)
    {
	PrintWriter out;
	for(int i = 0; i < routers.size() ; i++){
	    if(routers.get(i).getId().equals(id)){
		out = new PrintWriter(routers.get(i).getOutputStream(),true);
		if (out != null) {
		    out.println(message);
		    out.flush(); 
		}
	    }
	}
    }


    public void sendPaquet(String message,String idDestination)
    {
	for(int i = 0; i < routers.size() ; i++){
	    if(routers.get(i).getId().equals(idDestination)){
		routers.get(i).getRouterThread().sendPaquet(message,ttlValue,idDestination);
	    }
	}
    }
    

    synchronized public void delRouter(String id){
	for(int i = 0; i < routers.size() ; i++){
	    if(routers.get(i).getId().equals(id)){
		try{
		    sendMessage("bye",id);
		    routers.remove(i); 
		}catch(Exception e){}
		
	    }
	}
    }

    synchronized public int addRouter(RouterInfo routerInfo)
    {
	
	nbRouters++; 
	routers.add(routerInfo); 
	return routers.size()-1; 
    }
    
    synchronized public int getNbRouters()
    {
	return nbRouters; 
    }
    
    public void afficherRouters(){
	for(RouterInfo rI : this.routers){
	    System.out.println("afficher ID "+rI.getId());
	    System.out.println("afficher port "+rI.getPort());
	   	    
	}
    }
     
    public void delRouters(){
	System.out.println("delRouters");
	sendAll("bye");
	routers.clear();
    }
    
    public void enableTimeout(String id){
	for(int i = 0; i < routers.size() ; i++){
	    if(routers.get(i).getId().equals(id)){
		try{
		    routers.get(i).getSocket().setSoTimeout(defaultPacketTimeoutValue);
		}catch(Exception e){}
		
	    }
	}
    }

    public void disableTimeout(String id){
	for(int i = 0; i < routers.size() ; i++){
	    if(routers.get(i).getId().equals(id)){
		try{
		    routers.get(i).getSocket().setSoTimeout(0);
		}catch(Exception e){}
		
	    }
	}
    }

    public void sendPing(String id)
    {
	boolean found = false;
	for(int i = 0; i < routers.size() ; i++){
	    if(routers.get(i).getId().equals(id)){
		found = true;
		routers.get(i).getRouterThread().sendPing(this.ttlValue);
	    }
	}
	
	if(!found)
	    System.out.println("\terror: unknown destination !");
    }
    
    public boolean findRouter(String id){
	for(int i = 0; i < routers.size() ; i++){
	    if(routers.get(i).getId().equals(id)){
		return true;
	    }
	}
	return false;
    }

    public void connect(){
	try{
	    for(RouterInfo ri: routers){
		if(!alreadyConnected.contains(ri.getId())){
		    alreadyConnected.add(ri.getId());
		    System.out.println(ri.getId()+" "+ ri.getPort());
		    Socket s = new Socket("localhost", ri.getPort());
		    //Socket s = new Socket(ri.getIP(), ri.getPort());
		    ri.addSocket(s);
		    this.sendMessage("link id "+this.id+"*",ri.getId());
		    System.out.println("---------");
		    new RouterThread(s,this,Router.CLIENT,ri);
		}
	    }
	}catch(Exception e){
	    e.printStackTrace();}
    }
   
  

    private void parseConfigFile(String path){
	BufferedReader lect;
	String line = "";
	try {
			lect = new BufferedReader(new FileReader(path)) ;
			while (lect.ready()==true) {
			    line = lect.readLine();
			    String words[] = line.split(" ");
			    if(!line.equals("")) {
				switch (words[0]){
				case "router-port":
				    this.port = Integer.parseInt(words[2]);
				    break;
				case "controller-address":
				    this.controllerAd = this.StringToInetAddress(words[2]);
				    break;
				case "controller-port":
				    this.controllerPort = Integer.parseInt(words[2]);
				    break;
				case "controller-update-interval":
				    this.controllerUpdateInterval = Integer.parseInt(words[2]);
				    break;
				case "router-update-interval":
				    this.routerUpdateInterval = Integer.parseInt(words[2]);
				    break;
				case "default-ttl-value":
				    this.ttlValue = Integer.parseInt(words[2]);
				    break;
				case "default-ping-packet-count":
				    this.defaultPingPacketCount = Integer.parseInt(words[2]);
				    break;
				case "default-packet-timeout-value":
				    this.defaultPacketTimeoutValue = Integer.parseInt(words[2]);
				    break;
				case "default-dv-timeout-value":
				    this.defaultDVTimeoutValue = Integer.parseInt(words[2]);
				    break;
				default:
				    break;
				}
			    }
			}
	}
	catch (NullPointerException a){
	    System.out.println("Erreur : pointeur null");
	}
	catch (IOException a) {
	    System.out.println("Problème d'IO");
	}
	catch (Exception e) {
	    e.printStackTrace();
	}
    }
    
    public InetAddress StringToInetAddress(String IP) throws Exception{
	
	return InetAddress.getByName(IP);
      
      }
    
    
    public String constructVector(){
	String vector = "";
	if(!routers.isEmpty()){
	    vector = "vector ["+routers.get(0).getId()+","+routers.get(0).getCost();
	    if(routers.size() >  1){
		for(int i = 1; i < routers.size() ; i++)
		    vector += "; "+routers.get(i).getId()+","+routers.get(i).getCost();
		
		vector += "]*";
		
	    }
	}
	return vector;
    }
    
    synchronized public void routeTableUpdater(String Vector, String idSender){	
	StringTokenizer st = new StringTokenizer(Vector," *,;[]");
	System.out.println(st.nextToken());
	while(st.hasMoreTokens())
	    {
		String dest = st.nextToken();
		String cost = st.nextToken();
		if(idDes.contains(dest))
		    this.compareCost(dest,idSender,Integer.parseInt(cost));
		else{
		    idDes.add(dest);
		    this.cost.add(Integer.parseInt(cost));
		    idNext.add(idSender);
		}
	    }
	this.afficherRouteTable();
	
    }
    
    public void initRouteTable(){
	
	for(int i = 0; i < routers.size() ; i++){
	    
	    idDes.add(routers.get(i).getId());
	    cost.add(routers.get(i).getCost());    
	    if(routers.get(i).getId().equals(this.id))
		idNext.add("-");
	    else
		idNext.add(routers.get(i).getId());
	}	
    }
    
    private void compareCost(String iDDes,String idSender,int senderCost){
	int oldCost = 0 ;
	int newCost = senderCost ;
	for(int i = 0; i < idDes.size() ; i++){
	    if(idDes.get(i).equals(iDDes)){
		oldCost = cost.get(i);
	    }
	    if(idDes.get(i).equals(idSender))
		newCost += cost.get(i);
	}	
    }
    private void afficherRouteTable(){
	for(int i = 0; i < idDes.size() ; i++){
	    System.out.println("idDes "+idDes.get(i)+"\tidNext "+idNext.get(i)+"\tCost "+cost.get(i));
	}
    }

    
}
