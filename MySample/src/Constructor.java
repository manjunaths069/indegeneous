class another
{

		int x,y;
		another(int x,int y)
		{
			this.x=x;
			this.y=y;
		}
		another(){};
		
		int area()
		{
			return(x*y);
		}


}

public class Constructor
{

		public static void main(String args[])
		{
		
			another b = new another();
			b.x=2;b.y=4;
			System.out.println("Area of a rectangle without constructor is "+b.area());
		
			b = new another(5,6);
			System.out.println("Area of a rectangle with constructor is "+b.area());
		
		}
}