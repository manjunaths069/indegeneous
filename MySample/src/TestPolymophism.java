import java.util.Collections;
class base
{
	int baseTest;
	public void testBase()
	{
		System.out.println("Hi this is base class");
	}
}

class derived extends base
{
	Collections c = null;
	int t;
	
	int derTest;
	static int NAVEEN ;
	Object o = new Object();
	
	public void testBase()
	{
		System.out.println("Hi this is derived class ");
		System.out.println("Hi this is derived class " +t);	
	}
	void testDerived()
	{
		System.out.println("Hi this is derived methods");
	}
	static class test{
		void go(){test1();}
		
	}
	public static void test1()
	{
		derived.test t = new derived.test();
		t.go();
	}
	public static void main(String args[])
	{
		derived.test te = new derived.test();
		//System.out.println(te.go());
	}
    	
}

public class TestPolymophism {
	
	public static void main(String args[])
	{
		base b = new derived();
		try
		{
			int t;
		}catch(Error e)
		{
			e.printStackTrace();
		}
		
		((derived) b).testDerived();
		base b1 = new base();
		derived d = (derived)b1;
		d.testDerived();
		System.out.println("Calling polymorphic methos");
		b.testBase();
		System.out.println("Calling Derived class methods"+ derived.NAVEEN);
		//b.testDerived();
		
		
	}
}
