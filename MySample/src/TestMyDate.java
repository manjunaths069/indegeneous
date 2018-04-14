public class TestMyDate {
    public static void main(String[] args) {
        MyDate my_birth = new MyDate(22, 7, 1964);
        
        MyDate the_next_week = my_birth.addDays(7);        
        MyDate anotherDate = my_birth;
        
        System.out.println(the_next_week);
        System.out.println(my_birth);
        System.out.println(anotherDate);
    }
}
