import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;


public class TestReflection {

	public static void main(String args[])
	{
		try
		{
			Class c1 = Class.forName("java.lang.Object");
			
			Constructor cnst[] = c1.getConstructors();
			
			Field fld[] = c1.getFields();
			
			Method mtd[] = c1.getMethods();
			
			System.out.println("Name of the Constructors of the String class");
			
			for(int i=0;i<cnst.length;i++)
			{
				System.out.println(cnst[i].getName());
			}
			
			System.out.println("Name of the Declared fields");
			
			for(int i=0;i<fld.length;i++)
			{
				System.out.println(fld[i].getName());
			}

			System.out.println("Name of the Declared Methods");
			
			for(int i=0;i<mtd.length;i++)
			{
				System.out.println(mtd[i].getName());
			}

	
			
		}
		catch(ClassNotFoundException e)
		{
			e.printStackTrace();
		}
	}
	
	
}
