import java.util.HashMap;
import java.util.Map;


class Recurssion
{ 

	int fact (int n) { if (n==1) return 1; else return (fact(n-1)*n); }
	
	public static void main (String argS[]) 
	{ 
		Map m = new HashMap();
		m.entrySet();
		m.clear();
		Recurssion f =new Recurssion(); 
		System.out.println(" 6! = " + (f.fact(6))); 

		int a[] = {5,9,5,7,4,34,767,98,96,87,25,96,14,58,65478,85745,6974,6,8,1,7,25};
		int max = 0;
		for (int i = 0; i<a.length; i++)
		{
			if(a[i] > max)
				max= a[i];
		}
		System.out.println("maximum number is "+max);
	}
} 
