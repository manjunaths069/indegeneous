
class exam
{

 protected String test="Difficult";

 void print()
 {
    System.out.println(test);
 }
 
}

public class ex1 extends exam
{

	private String test="easy";

public static void main(String args[])
{
	ex1 e = new ex1();
	e.print();

	System.out.println(e.toString());
	String x = "big surprise";
	System.out.println( x.toString() );
	
	
	String s1="hello";
	String s2=new String ("hello");
	if(s1==s2)
		System.out.println("strings are same");
	else
		System.out.println("unequal");

	if(s1.equals(s2))
		System.out.println("Pass");
	else
		System.out.println("Fail");

	
	String s3 ="hello";
	String s4 =s3;
	if(s3 == s4)
		System.out.println("strings are same");
	else
		System.out.println("unequal");

	
}
}
