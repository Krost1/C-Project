import java.util.HashMap;
import java.util.Map;
import java.util.function.Supplier;

public class carte extends Case{
    private static Map<Character, Supplier<Case>> caseType = new HashMap<>();
    private static Map<Character, Supplier<Bombes>> bombeType = new HashMap<>();

    // Exemple d'utilisation : cases[i][j] = caseType.get('*').get();
    public Case[][] cases;

    public void Carte(int rows, int cols) {
        cases = new Case[rows][cols];
        prepareMap();
        prepareEntiteMap();
    }

    private void prepareMap() {
        caseType.put('*', MurBlinde::new);  // Utilisation d'une référence de méthode pour le constructeur
        caseType.put('=', Mur::new);
        caseType.put('-', CaseLibre::new);
    }

    private void prepareEntiteMap() {
        bombeType.put('B', ClassicBomb::new);
        bombeType.put('R', RemoteBomb::new);
        bombeType.put('M', Mine::new);
    }
}



