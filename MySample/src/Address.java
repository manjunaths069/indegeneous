public class Address {
    private String street;
    private String city;
    private String state;
    private String zipCode;
    
    public Address(String street, String city, String state, String zipCode) {
        this.street = street;
        this.city = city;
        this.state = state;
        this.zipCode = zipCode;
    }
    
    public String toString() {
        return street + "\n" + city + ", " + state + " " + zipCode;
    }
    public static void main(String args[]){
    	Address a = new Address("10","C B Pur","KA","562101");
    	a.toString();
    }
}
