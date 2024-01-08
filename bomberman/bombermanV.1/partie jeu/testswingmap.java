import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import javax.imageio.ImageIO;

public class testswingmap extends JFrame {
    private static final int CELL_SIZE = 30;

    private char[][] map = {
            "************************".toCharArray(),
            "=----------------------=".toCharArray(),
            "=----==============----=".toCharArray(),
            "=----------****--------=".toCharArray(),
            "=------****------------=".toCharArray(),
            "=----==============----=".toCharArray(),
            "=----------------------=".toCharArray(),
            "************************".toCharArray()
    };

    private int playerRow = 1; // Position initiale du joueur (ligne)
    private int playerCol = 1; // Position initiale du joueur (colonne)

    private BufferedImage SpriteMur;
    private BufferedImage SpriteMurInvincible;
    private BufferedImage bombSprite;

    private ArrayList<Bomb> bombs = new ArrayList<>();

    public testswingmap() {
        setTitle("Map Visualization");
        setSize(map[0].length * CELL_SIZE, map.length * CELL_SIZE);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLocationRelativeTo(null);

        loadSprites();

        add(new PlayerAndWallsPanel());
        addKeyListener(new KeyHandler());
        setFocusable(true);

        Timer timer = new Timer(1000, e -> updateBombs());
        timer.start();

        setVisible(true);
    }

    private void loadSprites() {
        try {
            SpriteMur = ImageIO.read(new File("Sprites/mur_cassable.png"));
            SpriteMurInvincible = ImageIO.read(new File("Sprites/mur_invincible.png"));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private class PlayerAndWallsPanel extends JPanel {
        private void drawSpriteMur(Graphics g, int row, int col, char cellType) {
            int x = col * CELL_SIZE;
            int y = row * CELL_SIZE;

            BufferedImage spriteToDraw;
            if (cellType == '*') {
                spriteToDraw = SpriteMurInvincible;
            } else if (cellType == '=') {
                spriteToDraw = SpriteMur;
            } else if (cellType == '-') {
                Color customColor = new Color(0, 150, 0);

                g.setColor(customColor);
                g.fillRect(x, y, CELL_SIZE, CELL_SIZE);
                return;
            } else {
                return;
            }

            if (spriteToDraw != null) {
                g.drawImage(spriteToDraw, x, y, CELL_SIZE, CELL_SIZE, this);
            } else {
                g.setColor(Color.BLUE);
                g.fillRect(x, y, CELL_SIZE, CELL_SIZE);
            }
        }

        public PlayerAndWallsPanel() {
            try {
                bombSprite = ImageIO.read(new File("Sprites/classic_bomb.png"));
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        private void drawBomb(Graphics g, Bomb bomb) {
            int x = bomb.getCol() * CELL_SIZE;
            int y = bomb.getRow() * CELL_SIZE;
    
            g.drawImage(bombSprite, x, y, CELL_SIZE, CELL_SIZE, this);
        }

        @Override
        protected void paintComponent(Graphics g) {
            super.paintComponent(g);

            for (int row = 0; row < map.length; row++) {
                for (int col = 0; col < map[row].length; col++) {
                    char cellType = map[row][col];
                    drawSpriteMur(g, row, col, cellType);
                }
            }

            for (Bomb bomb : bombs) {
                drawBomb(g, bomb);
            }

            int x = playerCol * CELL_SIZE;
            int y = playerRow * CELL_SIZE;
            g.setColor(Color.YELLOW);
            g.fillOval(x, y, 30, 30);
            g.setColor(Color.BLACK);
            g.fillOval(x + 8, y + 8, 4, 4);
            g.fillOval(x + 18, y + 8, 4, 4);
            g.drawArc(x + 8, y + 12, 14, 8, 0, -180);
        }
    }

    private class KeyHandler implements KeyListener {
        @Override
        public void keyPressed(KeyEvent e) {
            int newRow = playerRow;
            int newCol = playerCol;

            switch (e.getKeyCode()) {
                case KeyEvent.VK_LEFT:
                    newCol -= 1;
                    break;
                case KeyEvent.VK_RIGHT:
                    newCol += 1;
                    break;
                case KeyEvent.VK_UP:
                    newRow -= 1;
                    break;
                case KeyEvent.VK_DOWN:
                    newRow += 1;
                    break;
                case KeyEvent.VK_SPACE:
                    bombs.add(new Bomb(playerRow, playerCol));
                    break;
            }

            // Vérifiez si le nouveau déplacement est autorisé
            if (isMoveAllowed(newRow, newCol)) {
                playerRow = newRow;
                playerCol = newCol;

                repaint();
            }
        }

        @Override
        public void keyTyped(KeyEvent e) {
        }

        @Override
        public void keyReleased(KeyEvent e) {
        }
    }

    private boolean isMoveAllowed(int row, int col) {
        // Vérifiez si le déplacement du joueur est autorisé en ne traversant pas les murs
        return row >= 0 && row < map.length && col >= 0 && col < map[0].length
                && map[row][col] != '*' && map[row][col] != '=';
    }

    private void updateBombs() {
        ArrayList<Bomb> bombsToRemove = new ArrayList<>();

        for (Bomb bomb : bombs) {
            bomb.decrementTimer();

            if (bomb.getTimer() <= 0) {
                bombsToRemove.add(bomb);
            }
        }

        bombs.removeAll(bombsToRemove);

        repaint();
    }

    private class Bomb {
        private int row;
        private int col;
        private int timer;

        public Bomb(int row, int col) {
            this.row = row;
            this.col = col;
            this.timer = 4;
        }

        public int getRow() {
            return row;
        }

        public int getCol() {
            return col;
        }

        public int getTimer() {
            return timer;
        }

        public void decrementTimer() {
            timer--;
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new testswingmap());
    }
}
