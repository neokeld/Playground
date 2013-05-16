import java.net.*;
import java.io.*;
class ServerToClient extends Thread{
    
    Socket clientSocket;
    ServerToClient(Socket cs){
	clientSocket = cs;
	start();
    }
    
    public void run(){
	
	try{
	    PrintWriter out = new PrintWriter(clientSocket.getOutputStream(),true);
	    BufferedReader in = new BufferedReader(new InputStreamReader(System.in));	
	    String s;
	    
	    while((s = in.readLine())!=null)
		{
		    out.println(s);
		}
	}
	catch(IOException e){}		
	
	
    }
}