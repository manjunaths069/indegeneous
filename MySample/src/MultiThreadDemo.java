
class NewThread1 implements Runnable {
  String name = "getClusterNames_SP "; // name of thread

  Thread t;

  NewThread1(String threadname) {
    name = threadname;
    t = new Thread(this, name);
    //System.out.println("New thread: " + t);
    t.start(); // Start the thread
  }

  public void run() {
    try {
     // for (int i = 5; i > 0; i--) {
        //System.out.println(name + ": " );
        DBTest dbTest = new DBTest();
        dbTest.doWork(1);
        //Thread.sleep(1000);
      //}
    } catch (Exception e) {
      System.out.println(name + "Interrupted");
    }
    //System.out.println(name + " exiting.");
  }
}

class NewThread2 implements Runnable {
	  String name = "getHostNames_SP "; // name of thread

	  Thread t;

	  NewThread2(String threadname) {
	    name = name + threadname;
	    t = new Thread(this, name);
	   // System.out.println("New thread: " + t);
	    t.start(); // Start the thread
	  }

	  public void run() {
	    try {
	      //for (int i = 5; i > 0; i--) {
	       // System.out.println(name + ": " );
	        DBTest dbTest = new DBTest();
	        dbTest.doWork(2);
	       // Thread.sleep(1000);
	      //}
	    } catch (Exception e) {
	      System.out.println(name + "Interrupted");
	    }
	    //System.out.println(name + " exiting.");
	  }
	}

class NewThread3 implements Runnable {
	  String name =  "getDataStoreNames_SP "; // name of thread

	  Thread t;

	  NewThread3(String threadname) {
	    name = name + threadname;
	    t = new Thread(this, name);
	    //System.out.println("New thread: " + t);
	    t.start(); // Start the thread
	  }

	  public void run() {
	    try {
	      //for (int i = 5; i > 0; i--) {
	       // System.out.println(name + ": " );
	        DBTest dbTest = new DBTest();
	        dbTest.doWork(3);
	        //Thread.sleep(1000);
	     // }
	    } catch (Exception e) {
	      System.out.println(name + "Interrupted");
	    }
	   // System.out.println(name + " exiting.");
	  }
	}

class MultiThreadDemo {
  public static void main(String args[]) {
	  for (int i = 1; i <= 50 ; i++)
	  {
		    new NewThread1(i + " - Thread one"); // start threads
		    new NewThread2(i + " - Thread Two");
		    new NewThread3(i + " - Thread Three");
		  
		  /*Runnable rn = new Runnable(){
			public void run(){
				DBTest dbTest = new DBTest();
		        dbTest.doWork(1);
			}		  Thread t = new Thread(rn);
		  t.start();
		  };*/
		  

	  }
    try {
      //Thread.sleep(10000);
    } catch (Exception e) {
      System.out.println("Main thread Interrupted");
    }

    System.out.println("Main thread exiting.");
  }
}



