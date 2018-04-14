
public class myThread {

	public  static void main(String args[])
	{
		System.out.println("This thread example");
		
		threadTest te = new threadTest();
		Thread t = new Thread(te);
		
		t.run();
	}
	
}

class threadTest implements  Runnable
{
	public void run(){
		
		int i = 0;
		while (i <= 10){
			System.out.println("This thread example "+i); 
			i++;
		}
	}
	
}
