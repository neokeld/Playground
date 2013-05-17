import java.net.*;
import java.io.*;
import java.util.*;
import java.text.SimpleDateFormat;



class PeriodicVector implements Runnable{

    private Router router;
    private Thread t;
    private long routerUpdateInterval;
    
    public PeriodicVector(Router router, int routerUpdateInterval){
	this.router = router;
	this.routerUpdateInterval = routerUpdateInterval;
	t = new Thread(this);
	t.start();
    }
    

    public void run(){
	long start = System.currentTimeMillis()/1000;
	while(true){
	    long end = System.currentTimeMillis()/1000;
	    while(end - start < routerUpdateInterval){
		end = System.currentTimeMillis()/1000;
	    }
	    router.sendAll(router.constructVector());
	    start = end;
	}
    }
}