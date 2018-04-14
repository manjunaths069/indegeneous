public class Student {
    private int studentID;
    private String firstName;
    private String lastName;
    private Address address;
    
    public Student(int id, String fn, String ln, Address addr) {
        studentID = id;
        firstName = fn;
        lastName = ln;
        address = addr;
    }
    
    public int getStudentID() {
        return studentID;
    }
    
    public String getFirstName() {
        return firstName;
    }
    
    public String getLastName() {
        return lastName;
    }
    
    public Address getAddress() {
        return address;
    }
    
    public String toString() {
        return firstName + " " + lastName + "\n" + address.toString();
    }
}
