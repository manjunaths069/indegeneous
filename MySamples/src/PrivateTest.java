class TestTest{
	
	private int test ;

	public int getTest()
	{
		return test;
	}
	
	public void setTest(int test){
		
		this.test = test;		
	}

}


public class PrivateTest extends TestTest{

		public static void main(String args[]){
		
		String myString = "Naveen";
		myString.charAt(0);
		
		PrivateTest pt = new PrivateTest();
		System.out.println("This is test "+ pt.getTest());
	}
	
}


