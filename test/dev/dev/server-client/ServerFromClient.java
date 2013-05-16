import java.net.*;
import java.io.*;
class ServerFromClient extends Thread{
    
    Socket socket;		
    ServerFromClient(Socket ss){
	socket = ss;
	start();
    }
    
    public void run(){
	try{
	    BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
	    String s;		
	    while(( s = in.readLine())!=null)
		{
		    System.out.println("Client says:"+s);
		}
	}catch(IOException e){}
    }
    
}