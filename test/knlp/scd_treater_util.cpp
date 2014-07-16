#include "knlp/scd_treater.h"

using namespace std;

int main(int argc, char** argv)
{
	if (argc != 5) {
		cout << "./scd_treater_util  token_dict_path  knn_model_path  input_scd_file  output_scd_file" << endl;
		return 0;
	}
	
	SCD_treater st(argv[1], argv[2]);
	st.add_knn_field_to_scd(argv[3], argv[4]);
}


