import java.net.*;
import java.io.*;
import java.util.*;
import java.net.InetAddress;
public class RouterInfo {//implements Comparable {
    
    private Socket s;
    private String id;
    private RouterThread rt;
    private int cost;
    private InetAddress IP;
    private int port;
    
    public RouterInfo(){
    }
    public RouterInfo(Socket s,String id ){
	this.s =s;
	this.id = id;
    }
    
    public RouterInfo(Socket s){
	this.s =s;
    }
    public void addid(String id){
	this.id = id;
    }
    
    public void addPort(int port){
	
	this.port = port;
    }
    
    public void addRouterThread(RouterThread rt){
	this.rt = rt;
    }
    
    public void addCost(int cost){
	this.cost = cost;
    }
    public void addIP(InetAddress ip){
	this.IP = ip;
    }
    
    public String getId(){
	return this.id;
    }

    public InetAddress getIP(){
	return this.IP;
    }
    
    public void addSocket(Socket s){
	this.s = s;
    }
    
    public int getPort(){
	return this.port;
    }
    public int getCost(){
	return this.cost;
    }
    
    public RouterThread getRouterThread(){
	return this.rt;
    }
    
    public OutputStream getOutputStream(){
	try{
	    return this.s.getOutputStream();
	}catch(IOException e){ return null;}
    }
    public InputStream getInputStream(){
	try{
	    return this.s.getInputStream();
	}catch(IOException e){ return null;}
    }
    public Socket getSocket(){
	return this.s;
    }
    /*   
    public int compareTo(Object other){
	RouterInfo otherRI = (RouterInfo)other;
	if(this.id < otherRI.id)
	    return -1;
	else if(this.id > otherRI.id)
	    return 1;
	else
	    return 0;
    }
    */
    public boolean equals(Object other){
	RouterInfo otherRI = (RouterInfo)other;
	if(this.id.equals(otherRI.id))
	    return true;
	return false;
    }
}
