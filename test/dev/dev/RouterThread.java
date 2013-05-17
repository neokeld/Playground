
import java.net.*;
import java.io.*;
import java.util.*;
import java.text.SimpleDateFormat;
import java.lang.Math;

class RouterThread implements Runnable
{
    final static int MAXSEQNUM = 10;
    private Thread t; 
    private Socket s; 
    private PrintWriter out; 
    private BufferedReader in; 
    private Router router; 
    private int numRouter=0; 
    private RouterInfo connectedRI;
    private long ttl[];
    private long ttlPacket[];
    private boolean receivedPing[];
    private boolean receivedPong[];
    private List<Integer> seqNum;
    private List<String> dest;
    private List<ParsePacket> packets;
    private int nextPing;
    private int totalPings;
    private int success;
    private int failure;
    private boolean sendingPing;
    private boolean waitingACK;
    private long max;
    private long min;
    private long avg;
    private int seqNumCounter;
    private int timeOut;

    RouterThread(Socket s, Router router,int job,RouterInfo R) {
	this.router = router; 
	this.s = s; 
	nextPing = 0;
	totalPings = router.defaultPingPacketCount;
	timeOut = router.defaultPacketTimeoutValue;
	sendingPing = false;
	waitingACK = false;
	seqNumCounter = 0;
	seqNum = new ArrayList<Integer>();
	packets = new ArrayList<ParsePacket>();
	dest = new ArrayList<String>();
	ttl = new long[RouterThread.MAXSEQNUM];
	try{
	    in = new BufferedReader(new InputStreamReader(s.getInputStream()));
	    out = new PrintWriter(s.getOutputStream(),true);
	    String message = in.readLine();
	    
	    if(job == Router.SERVER){
		int index1 = message.lastIndexOf(' ');
		if(message.substring(0,index1).equalsIgnoreCase("link id")){
		    int index2 = message.indexOf('*',index1);
		    String id = message.substring(index1+1,index2);
		    connectedRI = new RouterInfo();
		    connectedRI.addid(id);
		    connectedRI.addSocket(s);
		    connectedRI.addRouterThread(this);
		    		    
		}
		router.addRouter(connectedRI);
		router.alreadyConnected.add(connectedRI.getId());
		router.sendMessage("link ok*",connectedRI.getId());
	    }

	    if(job == Router.CLIENT){
		R.addRouterThread(this);
		connectedRI = R;
		if(!message.equalsIgnoreCase("link ok*")){
		    router.sendMessage("I didn't receive your ID",connectedRI.getId());
		}	
	    }
	}catch (IOException e){
	    e.printStackTrace();}
	t = new Thread(this);
	t.start();
	
    }
	
  public void run()
  {
      try{
	  String message; 
	  System.out.println("Connection successful with router "+connectedRI.getId());
	  do{
	      
	      message = in.readLine();
	      if(message.equals("bye"))
		  break;
	      String split[] = message.split(" "); 
	      long startSendingAt;
	      
	      if(split[0].equalsIgnoreCase("ping")){
		  ParsePing pi = new ParsePing();
		  pi = pi.parsing(message);
		  regularSendPong(pi.getNumseq(),pi.getTtl());
	      }
	      
	      else if(split[0].equalsIgnoreCase("pong")){
		  
		  if(sendingPing == true){
		      ParsePing pi = new ParsePing();
		      pi = pi.parsing(message);
		      int seqNum = pi.getNumseq();
		      receivedPong[seqNum] = true;
		      long ttr = System.currentTimeMillis() - ttl[seqNum];
		      if(ttr < timeOut){
			  max = Math.max(max,ttr);
			  min = Math.min(min,ttr);
			  avg += ttr;
			  success++;
		      }
		      else
			  failure++;
		      if(nextPing < totalPings){
			  regularSendPing(pi.getTtl() - 1);
		      }
		      else {
			  sendingPing = false;
			  System.out.println("\tResult: "+success+" success, "+failure+" failure");
			  System.out.println("\tRTT   : min="+min+" avg="+avg/totalPings+" max="+max);
		      }
		  }
	      }
	      
	      else if(split[0].equalsIgnoreCase("message") && split[1].equalsIgnoreCase("received*")){
		  int index = message.lastIndexOf('R');
		  int index1 = message.indexOf(' ',index + 1);
		  startSendingAt = Long.parseLong(message.substring(index+4));
		  long endSendingAt = System.currentTimeMillis();
		  
		  if(endSendingAt - startSendingAt < timeOut)
		      System.out.println("\tmessage delivered ! RTT = "+ (endSendingAt - startSendingAt));
		  
		  else
		      System.out.println("\terror : message not delivered ! (Delta > X seconds)");
	      }
	      
	      else if(split[0].equalsIgnoreCase("packet")){
		  ParsePacket p = new ParsePacket();
		  long endSendingAt = System.currentTimeMillis();
		  p = p.parsing(message);
		  if(p.getData().equals("toofar*"))
		      System.out.println("\terror: unknown destination !");
		  else if(p.getData().equals("ok*")){
		      for(ParsePacket ps:packets){
			  if(ps.getNumseq()  == p.getNumseq()){
			      System.out.println("\tmessage delivered ! RTT = "+ (endSendingAt - ps.getTtl()));
			      packets.remove(ps);
			      break;
			  }
		      }
		  }
		  else {
		      if(p.getIdDest().equals(router.id)){
			  System.out.println(message);
			  out.println(p.constractPacketOk(p.getNumseq(),router.id,connectedRI.getId()));
			  System.out.println(p.getData());
		      }
		      else
			  this.sendPaquet(p.getData(),p.getTtl(),p.getIdDest());
			      
			  }
		      
	      }
	      
	      else if(split[0].equals("vector")){
		  router.routeTableUpdater(message,connectedRI.getId());
		  //System.out.println("thread "+message);
	      }
	      else {
		  int index = message.lastIndexOf('R');
		  startSendingAt = Long.parseLong(message.substring(index+4));
		  System.out.println("Router "+connectedRI.getId() +":"+ message.substring(0,index - 1));
		  router.sendMessage("message received*"+" RTT "+startSendingAt,connectedRI.getId());
	      }
	      
	      
	      
	  }while(!message.equals("bye"));
	  out.println("bye");
	  System.out.println("Closing connection with Router ID ="+connectedRI.getId());
	  router.delRouter(connectedRI.getId());
	  in.close();
	  s.close();
      }catch(Exception exception){
	  exception.printStackTrace();
      }     
  }


      void sendPing(long ttlValue){
	  nextPing = 0;
	  totalPings = 3;
	  sendingPing = true;
	  avg = 0;
	  success = 0;
	  failure = 0;
	  ttl = new long[totalPings];
	  receivedPing = new boolean[totalPings];
	  receivedPong = new boolean[totalPings];
	  ttl[nextPing] = System.currentTimeMillis();
	  max = 0;
	  min = router.defaultPacketTimeoutValue;
	  avg = 0;
	  receivedPing[nextPing] = true;
	  this.printTime();
	  ParsePing pi = new ParsePing();
	  out.println(pi.constractPing(nextPing, router.id, connectedRI.getId(),ttlValue));
	  nextPing++;
      }

    void sendMessage(){
	//nextSeqnum = 0;
    }
    
    void regularSendPing(long ttlValue){
	ttl[nextPing] = System.currentTimeMillis();
	receivedPing[nextPing] = true;
	this.printTime();
	ParsePing pi = new ParsePing();
	out.println(pi.constractPing(nextPing, router.id, connectedRI.getId(),ttlValue));
	nextPing++;
    }

    void regularSendPong(int seqNum,long ttlValue){
	ParsePing pi = new ParsePing();
	if(ttlValue != 0)
	    out.println(pi.constractPongTrue(seqNum, connectedRI.getId(),router.id,ttlValue));
	else
	    out.println(pi.constractPongFalse(seqNum, connectedRI.getId(),router.id));
    }
    
    private void printTime(){
	System.out.println("\tfrom "+router.id+" to "+connectedRI.getId()+" time ="+new SimpleDateFormat("HH:mm:ss").format(Calendar.getInstance().getTime()));
    } 

    public int getSeqNum(){
	if(seqNumCounter < RouterThread.MAXSEQNUM){
	    seqNumCounter++;
	}
	else
	    seqNumCounter = 0;
	return seqNumCounter;
    } 
    
    public void sendPaquet(String message,long ttl,String dest)
    {
	
	int seq = getSeqNum();
	ParsePacket p = new ParsePacket();
	if(ttl == 0)
	    out.println(p.constractPacketTooFar(seq, router.id, dest));
	else{
	    out.println(p.constractPacketData(seq, router.id, dest, ttl, message));
	}
	p.setNumseq(seq);
	p.setIdSrc(connectedRI.getId());
	p.setTtl(System.currentTimeMillis());
	packets.add(p);
	out.flush(); 
    }

    
	    
}
