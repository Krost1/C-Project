public class BomberStudent {
    public int points_de_vie;
    public int vitesse;
    public int distance_Impact;
    public int nb_classicbomb;
    public int nb_remotebomb;
    public int nb_mine;
    public boolean invincible;   
    
    
    // Constructeur
    public BomberStudent(int vieInitiale, int vitesseInitiale, int impactInitiale, int nb_classicbomb, int nb_remotebomb, int nb_mine, boolean invincible) {
        this.points_de_vie = vieInitiale;
        this.vitesse = vitesseInitiale;
        this.distance_Impact = impactInitiale;
        this.invincible = invincible;
    }
    

    // Méthode pour augmenter la vitesse
    public void augmenterVitesse(int bonusVitesse) {
        this.vitesse += bonusVitesse;
    }

    public void diminuerVitesse(int malusVitesse) {
        this.vitesse -= malusVitesse;
        // Ajoutez d'autres logiques si nécessaire, par exemple pour éviter une vitesse négative
    }

    public void devenirInvincible() {
        this.invincible = true;
        // Ajoutez d'autres logiques si nécessaire
    }

    public void augmenterImpact(int bonusImpact) {
        this.distance_Impact += bonusImpact;
    }

    public void diminuerImpact(int malusImpact) {
        this.distance_Impact -= malusImpact;
    }

    public void augmenterLifeMax(int bonusVie) {
        this.points_de_vie += bonusVie;
        // Vous pouvez ajouter une logique supplémentaire ici, par exemple ajuster les points_de_vie si nécessaire
    }
}