
public class TestDiamond implements test1,test2{

	public void doTest()
	{
		System.out.println("Hi Just Test");
				
	}
	public static void main(String args[])
	{
		TestDiamond t = new TestDiamond();
		t.doTest();
		if(true)
			System.out.println();
		
		//String s =new String("Test");
	
	}
	
}

interface test1
{
	 public void doTest();
}

interface test2
{
	 public void doTest();
}
