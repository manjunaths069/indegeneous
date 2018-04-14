interface testChange
{
	public void myTest(); 
	class my
	{
		int myTest=-1;
		void myTestFunc()
		{
			System.out.println("Hi");
		}
		
	}
}

public class TestStudent implements testChange {
    public static void main(String[] args) {
        Address addr = new Address("One Java Place", "HomeTown", "State", "12345");        
        Student cust = new Student(1, "Java", "Duke", addr);
        
        System.out.println(cust);
        
        testChange.my m = new testChange.my();
        m.myTestFunc();
    }
    public void myTest()
    {
    	System.out.println("Inteface method");
    }
}

class testClass 
{
	int mytestClass = -1;
	void testClass2()
	{
		System.out.println("My Class with interface");
	}
	
	interface testClassIntf
	{
		void testHi();
	}
}
