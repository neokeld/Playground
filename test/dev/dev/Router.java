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
    public List<RouterInfo> routers;
    public List<RouterInfo> traceRoute;
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


    public List<String> idDes;
    public List<String> idNext;
    public List<Integer> cost;


    private void launch(String id, Router router, String path){
	this.id = id;
	controllerInfo = new RouterInfo();
	router.parseConfigFile(path);
	listId = new ArrayList<Integer>();
	idDes = new ArrayList<String>();
	idNext = new ArrayList<String>();
	cost = new ArrayList<Integer>();
	alreadyConnected = new ArrayList<String>();
	routers = new ArrayList();
	traceRoute = new ArrayList();
	alreadyConnectedTo = new ArrayList();	
	try{
	  
	    ServerSocket ss = new ServerSocket(port);
	    Socket controlerSocket = new Socket(controllerAd,controllerPort);
	    //Socket controlerSocket = new Socket("localhost" ,controllerPort);
	    controllerInfo.addSocket(controlerSocket);
	    controlerIn = new BufferedReader(new InputStreamReader(controlerSocket.getInputStream()));
	    controlerOut = new PrintWriter(controlerSocket.getOutputStream(),true);
	    routerControler(controlerOut,controlerIn,id,port);
	    printWelcome(port,this.id);
	    new ControllerThread(controlerSocket,router);
	    new Commandes(router); 
	    new PeriodicVector(router,routerUpdateInterval);
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
	     alreadyConnected.add(this.id);
	 }catch(Exception e){ e.printStackTrace();}
	 
     }


    public static void main(String args[]){
	try{	
	    Router router = new Router(); 
	    int port;
	    String id;
	    id = args[0];
	    router.launch(id,router,args[1]);  
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

    public String getNext(String destination){
	int next = 0;
	for(int i = 0; i < idDes.size() ; i++){
	    if(idDes.get(i).equals(destination)){
		next = i;
	    }
	}
	return idNext.get(next);
    }

    public void sendMessage(String message,String id)
    {
	PrintWriter out = null;
	int next = 0;
	String destination;
	boolean found = false;
	for(int i = 0; i < idDes.size() ; i++){
	    if(idDes.get(i).equals(id)){
		next = i;
		found = true;
	    }
	}
	if(!found)
	    destination = id;
	else
	    destination = idNext.get(next);

	for(int i = 0; i < routers.size() ; i++){
	    if(routers.get(i).getId().equals(destination)){
		try{
		    out = new PrintWriter(routers.get(i).getOutputStream(),true);
		}catch(NullPointerException e){System.out.println("Null pointer "+routers.get(i).getId());}
		if (out != null) {
		    out.println(message);
		    out.flush(); 
		}
	    }
	}
    }
    

    public void sendPaquet(String message,String idDestination)
    {
	int next = 0;
	for(int i = 0; i < idDes.size() ; i++){
	    if(idDes.get(i).equals(idDestination)){
		next = i;
		
	    }
	}

	for(int i = 0; i < routers.size() ; i++){
	    if(routers.get(i).getId().equals(idNext.get(next))){
		routers.get(i).getRouterThread().sendPaquet(message,ttlValue,idDestination);
	    }
	}

    }
    

    public void delRouter(String id){
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
     
    synchronized public void delRouters(){
	sendAll("bye");
	alreadyConnected.clear();
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
	int next = 0;
	for(int i = 0; i < idDes.size() ; i++){
	    if(idDes.get(i).equals(id)){
		next = i;
		found = true;
	    }
	}

	for(int i = 0; i < routers.size() ; i++){
	    if(routers.get(i).getId().equals(idNext.get(next))){
		routers.get(i).getRouterThread().sendPing(this.ttlValue,id);
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

    public RouterInfo findRouters(String id){
	for(int i = 0; i < routers.size() ; i++){
	    if(routers.get(i).getId().equals(id)){
		return routers.get(i);
	    }
	}
	return null;
    }


    public void connect(){
	try{
	    for(RouterInfo ri: routers){
		if(!alreadyConnected.contains(ri.getId())){
		    alreadyConnected.add(ri.getId());
		    //Socket s = new Socket("localhost", ri.getPort());
		    Socket s = new Socket(ri.getIP(), ri.getPort());
		    ri.addSocket(s);
		    System.out.println(id+" added");
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
	if(!idDes.isEmpty() && idDes.size() >  1){
	    vector = "vector ["+idDes.get(1)+","+cost.get(1);
	    if(idDes.size() >  2){
		for(int i = 2; i < idDes.size() ; i++){
		    vector += "; "+idDes.get(i)+","+cost.get(i);		
		}
	    }
	    vector += "]*";
	}
	return vector;
    }

    synchronized public void routeTableUpdater(String Vector, String idSender){	
	int costNext = 0;
	StringTokenizer st = new StringTokenizer(Vector," *,;[]");
	if(st.hasMoreTokens())
	    st.nextToken();
	while(st.hasMoreTokens()){
	    String dest = st.nextToken();
	    String cost = st.nextToken();
	    if(idDes.contains(dest) && !dest.equals(this.id))
		this.compareCost(dest,idSender,Integer.parseInt(cost));
	    else if (!dest.equals(this.id)){
		idDes.add(dest);
		for(int i = 0; i < idDes.size() ; i++){
		    if(idDes.get(i).equals(idSender))
			costNext = this.cost.get(i);
		}
		idNext.add(idSender);
		this.cost.add(Integer.parseInt(cost)+costNext);
	    }
	}
    }
    
   synchronized  public void initRouteTable(){
	idDes.clear();
	idNext.clear();
	cost.clear();
	idDes.add(this.id);   
	idNext.add("-");
	cost.add(0);    
	for(int i = 0; i < routers.size() ; i++){
	    
	    idDes.add(routers.get(i).getId());   
	    cost.add(routers.get(i).getCost());    
	    idNext.add(routers.get(i).getId());
	    
	}	
    }
    
    private void compareCost(String iDDes,String idSender,int senderCost){
	int oldCost = 0;
	int newCost = senderCost ;
	int index = 0;
	for(int i = 0; i < idDes.size() ; i++){
	    if(idDes.get(i).equals(iDDes)){
		oldCost = cost.get(i);
		index = i;
	    }
	    if(idDes.get(i).equals(idSender)){
		newCost += cost.get(i);
		
	    }
	}
	if( newCost < oldCost){
	    idDes.remove(index);
	    cost.remove(index);
	    idNext.remove(index);
	    
	    idDes.add(iDDes);
	    idNext.add(idSender);
	    cost.add(newCost);
	}
    }
    public void afficherRouteTable(){
	for(int i = 0; i < idDes.size() ; i++){
	    System.out.println("idDes "+idDes.get(i)+"\tidNext "+idNext.get(i)+"\tCost "+cost.get(i));
	}
    }

    
}
