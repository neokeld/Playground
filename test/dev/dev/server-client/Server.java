import java.net.*;
import java.io.*;

public class Server {
    
    public static void main(String args[]) throws Exception{
	
	ServerSocket serverSocket = new ServerSocket(4444);
	Socket clientSocket = serverSocket.accept();
	
	new ServerToClient(clientSocket);
	new ServerFromClient(clientSocket);
	
	
	System.out.println("Courtsey Manu Gupta");
	
	
    }
}