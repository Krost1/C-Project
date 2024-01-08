public class Bonus_Malus {
    public int impact_up;
    public int impact_down;
    public int speed_up;
    public int speed_down;
    public boolean invincible;
    public int life_max;


    public void appliquerBonus(BomberStudent joueur) {
        if(impact_up > 0) {
            joueur.augmenterImpact(impact_up);
        } 

        if(speed_up > 0) {
            joueur.augmenterVitesse(speed_up);
        } 

        if(invincible) {
            joueur.devenirInvincible();
        }

        if (life_max > 0) {
            joueur.augmenterLifeMax(life_max);
        }
    }

    public void appliquerMalus(BomberStudent joueur) {
        if(impact_down > 0) {
            joueur.diminuerImpact(impact_down);
        }

        if(speed_down > 0) {
            joueur.diminuerVitesse(speed_down);
        }
    }
}