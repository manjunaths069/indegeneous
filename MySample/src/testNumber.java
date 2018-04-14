
public class testNumber {

	private int a = 4;
	private int b = 5;
	private int c = 6;
	
	private void findOther(int input)
	{
	
		int tempa  = a - input;
		int tempb  = b - input;
		int tempc  = c - input;
		System.out.println("The other number are ");
		if(tempa != 0)
			System.out.println(a );

		if(tempb != 0)
			System.out.println(b );

		if(tempc != 0)
			System.out.println(c );

		
	}
	
	public static void main(String args [])
	{
		int input = Integer.parseInt(args[0]);
		testNumber t = new testNumber();
		t.findOther(input);
		System.out.println(Math.floor(-3.22));
		System.out.println(Math.sqrt(-4));
		x myy = new y();
		byte b = 4;
		b++;
		
	}

}

class x
{
	
}

class y extends x
{
	
}
