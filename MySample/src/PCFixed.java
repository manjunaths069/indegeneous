import java.lang.*;
import java.lang.Thread;
class Q
{
	int n;
	boolean valueSet=false;
	synchronized int get()
	{
		if(!valueSet)
			try{
				wait();
			}
		catch(InterruptedException e)
		{
			System.out.println("InterruptedException catch");
		}
		System.out.println("got:"+n);
		valueSet=false;
		notify();
		return n;
	}
	synchronized void put(int n)
	{
		if(valueSet)
			try{
				wait();
			}
		catch(InterruptedException e)
		{
			System.out.println("InterruptedException catch");
		}
		this.n=n;
		valueSet=true;
		System.out.println("put:"+n);
		notify();
	}
}
class Producer implements Runnable
{
	Q q;
	Producer(Q q)
	{
		this.q=q;
		new Thread(this,"producer").start();
	}
	public void run()
	{
		int i=0;
		while(true)
		{
			q.put(i++);
		}
	}
}
class Consumer implements Runnable
{
	Q q;
	Consumer(Q q)
	{
		this.q=q;
		new Thread(this,"consumer").start();
	}
	public void run()
	{
		while(true)
		{
			q.get();
		}
	}
}
class PCFixed
{
	public static void main(String args[])
	{
		Q q=new Q();
		new Producer(q);
		new Consumer(q);
	}
}

