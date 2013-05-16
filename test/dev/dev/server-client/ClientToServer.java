import java.net.*;
import java.io.*;
class ClientToServer extends Thread{
    Socket clientSocket;
    
    ClientToServer(Socket socket){
	clientSocket = socket;
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
	}catch(IOException e){}		
    }
}