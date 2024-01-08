import java.net.Socket;
import java.net.InetSocketAddress;

public class client {

    public static void main(String[] args) throws Exception {
        // Create a socket for connecting to the server
        Socket socket = new Socket();

        // Connect to the server on port 8080
        socket.connect(new InetSocketAddress("localhost", 1234));

        // Encode the request as a byte array
        byte[] requestBytes = "Demande".getBytes();

        // Send the request to the server
        socket.getOutputStream().write(requestBytes);

        // Close the socket
        socket.close();
    }
}