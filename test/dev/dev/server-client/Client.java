import java.net.*;
import java.io.*;

public class Client {
    
    public static void main(String args[]) throws Exception {
	Socket socket = new Socket("localhost",4444);
	
	new ClientFromServer(socket);
	new ClientToServer(socket);
	
	System.out.println("Courtsey Manu Gupta");
    }
}

