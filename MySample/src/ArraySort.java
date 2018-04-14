
public class ArraySort {

	int array[]= {0,1,0,1,0,1,1,1,1,1,0,0,1,0,1,0,1,1,0,0};
	
	public void sortArray(int a[])
	{
		for(int i=0;i<=a.length-1;i++)
		{
			for(int j=a.length-1;j>=i;j--)
			{
				if(a[i] > a[j])
				{
					int temp=a[i];
					a[i]=a[j];
					a[j]=temp;
				}
			}
			
		}
		
	}
	
	public void displayArray(int a[])
	{
		for(int i=0;i<a.length;i++)
			System.out.println("a["+i+"] ==>>" + a[i]);
		
	}
	public static void main(String args[])
	{
		ArraySort a = new ArraySort();
		
		System.out.println("Array before sorting");
		a.displayArray(a.array);
		a.sortArray(a.array);
		System.out.println("Array after sorting");
		a.displayArray(a.array);
				
	}
	
}
