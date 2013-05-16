import java.net.*;
import java.io.*;
import java.util.*;
import java.text.SimpleDateFormat;

class Commandes implements Runnable {
    Router router; 
    BufferedReader in;
    String strCommande=""; 
    Thread t; 
    
    
    Commandes(Router router){
	this.router = router; 
	in = new BufferedReader(new InputStreamReader(System.in));
	t = new Thread(this); 
	t.start(); 
    }
    
    
    public void run() {
	try{
	    while ((strCommande=in.readLine())!=null){
		String[] split = strCommande.split(" ");
		int index1 = strCommande.indexOf(' ');
		if (strCommande.equalsIgnoreCase("quit")){
		    router.controlerOut.println("log out*");
		    router.sendAll("bye");
		    System.exit(0); 
		}
		
		else if(strCommande.equalsIgnoreCase("total")){
		    System.out.println("Nombre de connectes : "+router.getNbRouters());
		    System.out.println("--------");
		}
		
		
		else if (strCommande.equalsIgnoreCase("connect")){
		    router.connect();
		}
		
		
		else if (strCommande.equalsIgnoreCase("show")){
		    router.afficherRouters();
		}
		
		else if (index1 != -1 && strCommande.substring(0,index1).equalsIgnoreCase("ping")){
		    String id = split[1];
		    System.out.println("commande  "+id);
		    router.sendPing(id);
		    
		}
		
		else if (index1 != -1 && strCommande.substring(0,index1).equalsIgnoreCase("message")){
		    int index3 = strCommande.indexOf(' ',index1+2);
		    int index2 = strCommande.indexOf('"',index1);
		    String id = strCommande.substring(index1+1,index3); 
		    long startSendingAt = System.currentTimeMillis();
		    if(router.findRouter(id)){
			this.printTime();
			router.sendPaquet(strCommande.substring(index2+1,strCommande.length()-1),id);
		    }
		    else
			System.out.println("\terror: unknown destination !");
		    
		    
		}
		
		
		/*else if (strCommande.equalsIgnoreCase("add port")){
		    System.out.println("-------");
		    System.out.println("Add new ports");
		    System.out.println("Tape end to finish");
		    strCommande = in.readLine();
		    while(!strCommande.equalsIgnoreCase("end")){
			router.listPort.add(new Integer(strCommande));
			System.out.println("port number "+router.listPort.get(router.listPort.size()-1)+" added");
			strCommande = in.readLine();
		    }
		    System.out.println("-------");
		    }*/
	

		/*else if(split[0].equals("neighborhood"))
		    new ControllerThread(router,strCommande);*/
			
		else{
		    System.out.println("Cette commande n'est pas supportee");
		    System.out.println("Quitter : \"quit\"");
		    System.out.println("Nombre de connectes : \"total\"");
		    System.out.println("--------");
		    }
		System.out.flush(); 
		
	    }    		    
	}catch (IOException e) {}
    }

    private void printTime(){
	System.out.println("\tmessage sent at "+new SimpleDateFormat("HH:mm:ss").format(Calendar.getInstance().getTime()));
    } 

    	
}
