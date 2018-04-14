/*public class Test {
    public static void main(String[] args) {
        Dog d = new Dog();
        d.setWeight(30);
        System.out.println("The dog has a weight of " + d.getWeight());
    }
}*/

interface TestInt {}
interface TestInt1{}
interface test extends TestInt,TestInt1 {}

 public class Test {
	 public static void main(String args[]) {
	 class Foo {
	 public int i = 3;
	 }
	 Object o = (Object)new Foo();
	 Foo foo = (Foo)o;
	 System.out.println("i = " + foo.i);
	 }
	 }
 

