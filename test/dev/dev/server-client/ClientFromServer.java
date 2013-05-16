import java.net.*;
import java.io.*;
class ClientFromServer extends Thread{
    
    Socket socket;	
    ClientFromServer(Socket s){
	socket = s;
	start();
    }
    
    public void run(){
	
	try{
	    BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
	    String s;		
	    while(( s = in.readLine())!=null)
		{
		    System.out.println("Server says:"+s);
		}
	}catch(IOException e){}
    }
    
}