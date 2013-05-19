import java.net.*;
import java.io.*;
import java.util.*;
import java.text.SimpleDateFormat;

class Commandes implements Runnable {
    
    static int HOP;
    Router router; 
    BufferedReader in;
    String strCommande=""; 
    Thread t; 
    
    
    Commandes(Router router){
	this.router = router; 
	in = new BufferedReader(new InputStreamReader(System.in));
	t = new Thread(this); 
	t.start();
	Commandes.HOP = 0;
    }
    
    
    public void run() {
	try{
	    while ((strCommande=in.readLine())!=null){
		String[] split = strCommande.split(" ");
		int index1 = strCommande.indexOf(' ');
		if (strCommande.equalsIgnoreCase("quit")){
		    router.controlerOut.println("log out*");
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
		else if (strCommande.equalsIgnoreCase("routetable")){
		    router.afficherRouteTable();
		}
		
		else if (index1 != -1 && strCommande.substring(0,index1).equalsIgnoreCase("ping")){
		    String id = split[1];
		    router.sendPing(id);
		    
		}
		
		else if (index1 != -1 && strCommande.substring(0,index1).equalsIgnoreCase("message")){
		    int index3 = strCommande.indexOf(' ',index1+2);
		    int index2 = strCommande.indexOf('"',index1);
		    String id = strCommande.substring(index1+1,index3); 
		    long startSendingAt = System.currentTimeMillis();
		    if(router.idDes.contains(id)){
			this.printTime();
			router.sendPaquet(strCommande.substring(index2+1,strCommande.length()-1),id);
		    }
		    else
			System.out.println("\terror: unknown destination !");
		    
		    
		}
		
		
		
		else if(split[0].equals("route")){
		    int next = 0;
		    Commandes.HOP = 1;
		    for(int i = 0; i < router.idDes.size() ; i++){
			if(router.idDes.get(i).equals(split[1])){
			    next = i;
			}
		    }
		    System.out.println("\t\tHop "+Commandes.HOP +" = "+router.idNext.get(next));
		    Commandes.HOP++;
		    router.sendMessage("where "+split[1]+" "+router.id,split[1]);
		}
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
