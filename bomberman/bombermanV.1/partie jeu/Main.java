public class Main {

    public class Case {
        // Représente chaque cases des maps
    }

    public class Mur extends Case{
        private int dureté;

        public Mur(int dureté) {
            this.dureté = dureté;
        }

        public int getDureté() {
            return dureté;
        }

        public void setDureté(int dureté) {
            this.dureté = dureté;
        }
    }

    public class MurBlindé extends Case{
        private int dureté;

        public MurBlindé(int dureté) {
            this.dureté = dureté;
        }
        public int getDureté() {
            return dureté;
        }

        public void setDureté(int dureté) {
            this.dureté = dureté;
        }
    }

    public class CaseLibre extends Case{
        private boolean libre;

        public boolean isLibre() {
            return libre;
        }

        public void setLibre(boolean libre) {
            this.libre = libre;
        }
    }

    public class Bombes {
        
    }

    public class ClassicBomb extends Bombes {
        
    }
}