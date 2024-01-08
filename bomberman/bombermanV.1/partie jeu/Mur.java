import java.util.Random;

public class Mur extends Case{
    public boolean estCasse;
    public Objet objet;

    public Mur() {
        this.estCasse = false;
        this.libre = false;
        this.objet = null; 
    }

    public void casserMur() {
        this.estCasse = true;
        this.libre = true;  // Le mur devient une case libre lorsqu'il est cassé

        // Générez un nombre aléatoire entre 0 et 99
        int randomDrop = new Random().nextInt(100);

        // Si le nombre généré est inférieur à 90, attribuez un objet
        if (randomDrop < 90) {
            this.objet = genererObjet();
        }
    }

    private Objet genererObjet() {
        // Implémentez la logique de génération de l'objet (bonus ou malus)
        // Retournez l'objet généré
        return new Objet();  // À adapter en fonction de votre implémentation d'Objet
    }
}