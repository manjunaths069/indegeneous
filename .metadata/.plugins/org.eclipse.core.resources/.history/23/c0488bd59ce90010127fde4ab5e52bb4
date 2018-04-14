

/*
class Person
{
String name = "No name";
public Person (String nm){ name = nm;}
}
class Employee extends Person {
		String empID = "0000";
		public Employee(String id){empID = id;}
}

public class EmployeeTest
{
	public static void main(String args[])
	{
		Employee e = new Employee("4321");
		System.out.println(e.empID);
	}

}*/


class Inner
{

	private int x;
	public void setX(int x)
	{
		this.x = x;
	}
	public int getX(){return x;}

}

class Outer
{

	private Inner y;

	public void setY(Inner y)
		{this.y = y;}
	
	public Inner getY(){ return y;}
}

public class EmployeeTest{

public static void main(String args[])
{
	Outer o = new Outer();
	Inner i = new Inner ();
	int n=10;
	i.setX(n);
	o.setY(i);
	//o.setY(i).setX(100);
	//i = new Inner();i.setX(100);
	//i = new Inner();i.setX(100);o.setY(i);
	System.out.println(o.getY().getX());


	}
}
