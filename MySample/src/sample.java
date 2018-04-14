import java.util.ArrayList;


public class sample {
	public	void testSample(Object o)
	{
		System.out.println("Hello");
	}

	
	public static void main(String args[])
	{

		
		System.out.println("Hi welcome to the world of JAVA");
		final ArrayList<Integer> a = new ArrayList<Integer>();
		
		ArrayList b = new ArrayList();
		
		b.add(20);
		
		
		Integer o = new Integer(20);
		a.add(o);
		
		new sample().testSample(null);
		try{
			int i=1;
			 
			i=i/0;
			 
		}
		catch(Exception e)
		{
			e.printStackTrace();
			return ;
		}
		finally
		{
			System.out.println("Hi");
			return ;
		}
	}
}
	


