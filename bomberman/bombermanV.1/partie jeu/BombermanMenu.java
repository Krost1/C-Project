import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.net.Socket;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class BombermanMenu extends JFrame {
    private static final String SERVER_IP = "127.0.0.1";
    private static final int SERVER_PORT = 42069;

    private static final String MAPS_LIST_COMMAND = "GET maps/list";
    private static final String MAPS_ALREADY_START = "GET game/list";
    private JTextArea mapTextArea;

    public BombermanMenu() {
        super("Bomberman Menu");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(500, 500);
        setLocationRelativeTo(null);

        JButton createGameButton = new JButton("Voir les parties en cours");
        JButton viewListButton = new JButton("Voir la liste des maps");
        JButton quitButton = new JButton("Quitter");

        createGameButton.setSize(100, 100);
        viewListButton.setSize(100, 100);
        quitButton.setSize(100, 100);

        mapTextArea = new JTextArea();
        mapTextArea.setEditable(false);
        JScrollPane scrollPane = new JScrollPane(mapTextArea);

        createGameButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                sendRequestToServer(MAPS_ALREADY_START);
            }
        });

        viewListButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                sendRequestToServer_two(MAPS_LIST_COMMAND);
            }
        });

        quitButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                System.out.println("Merci d'avoir joué à Bomberman. À bientôt !");
                System.exit(0);
            }
        });

        // Create panel and add buttons
        JPanel panel = new JPanel();
        panel.setLayout(new GridLayout(3, 1));
        panel.add(createGameButton);
        panel.add(viewListButton);
        panel.add(quitButton);

        add(scrollPane, BorderLayout.CENTER);
        add(panel, BorderLayout.SOUTH);

        setVisible(true);
    }

    private void afficherInformationsCarte() {
        // Votre code actuel pour afficher les informations de la carte
    }

    private void sendRequestToServer(String request) {
        try (Socket socket = new Socket(SERVER_IP, SERVER_PORT);
             PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()))) {
    
            out.println(request);
    
            StringBuilder response = new StringBuilder();
            String line;
            while ((line = in.readLine()) != null) {
                response.append(line);
            }
            JSONObject jsonObject = new JSONObject(response.toString());
    
            JSONArray gamesArray = jsonObject.getJSONArray("games");
    
            for (int i = 0; i < gamesArray.length(); i++) {
                JSONObject gameObject = gamesArray.getJSONObject(i);
    
                String name = gameObject.getString("name");
                int nbPlayers = gameObject.getInt("nbPlayer");
                int mapId = gameObject.getInt("mapId");
    
                mapTextArea.append("Nom de la partie : " + name + "\n");
                mapTextArea.append("Nombre de joueurs : " + nbPlayers + "\n");
                mapTextArea.append("ID de la carte : " + mapId + "\n");
                mapTextArea.append("\n");
            }
    
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }
    

    private void sendRequestToServer_two(String request) {
        try (Socket socket = new Socket(SERVER_IP, SERVER_PORT);
             PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()))) {

            out.println(request);

            StringBuilder response = new StringBuilder();
            String line;
            while ((line = in.readLine()) != null) {
                response.append(line);
            }
            JSONObject jsonObject = new JSONObject(response.toString());
            
            JSONArray jsa = jsonObject.getJSONArray("maps");
            
            // La boucle qui va afficher l'id et la map une par une
            for (int i = 0; i < jsa.length(); i++) {
                JSONObject mapObject = jsa.getJSONObject(i);

                int id = mapObject.getInt("id");
                String content = mapObject.getString("content");
                mapTextArea.append(id + ".");
                mapTextArea.append(content);
            }

        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new BombermanMenu());
    }
}
