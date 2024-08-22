import os
from dataset_keyword_generator import generate_dataset, generate_dataset_with_keywords
from query_keywords_generator import generate_query_keywords

if __name__ == "__main__":
    generate_dataset("realworld", "AM", "out.wang-amazon", "lognorm", 500, 3)
    generate_query_keywords("realworld", "AM", "keywords_list.txt", 5, 10)

    generate_dataset("realworld", "AR", "out.amazon-ratings", "lognorm", 500, 3)
    generate_query_keywords("realworld", "AR", "keywords_list.txt", 5, 10)

    generate_dataset_with_keywords("realworld", "BS", "out.bibsonomy-2ui", "out.bibsonomy-2ti")
    generate_query_keywords("realworld", "BS", "keywords_list.txt", 5, 10)

    generate_dataset("realworld", "CM", "out.librec-ciaodvd-movie_ratings", "lognorm", 500, 3)
    generate_query_keywords("realworld", "CM", "keywords_list.txt", 5, 10)

    generate_dataset_with_keywords("realworld", "CU", "out.citeulike-ui", "out.citeulike-ti")
    generate_query_keywords("realworld", "CU", "keywords_list.txt", 5, 10)

    generate_dataset("realworld", "DV", "out.digg-votes", "lognorm", 500, 3)
    generate_query_keywords("realworld", "DV", "keywords_list.txt", 5, 10)

    generate_dataset_with_keywords("realworld", "ML", "out.movielens-10m_ui", "out.movielens-10m_ti")
    generate_query_keywords("realworld", "ML", "keywords_list.txt", 5, 10)

    generate_dataset("realworld", "SX", "out.escorts", "lognorm", 500, 3)
    generate_query_keywords("realworld", "SX", "keywords_list.txt", 5, 10)

    generate_dataset("realworld", "TA", "out.wang-tripadvisor", "lognorm", 500, 3)
    generate_query_keywords("realworld", "TA", "keywords_list.txt", 5, 10)

    generate_dataset("realworld", "UF", "out.opsahl-ucforum", "lognorm", 500, 3)
    generate_query_keywords("realworld", "UF", "keywords_list.txt", 5, 10)

    generate_dataset_with_keywords("realworld", "VU", "out.pics_ui", "out.pics_ti")
    generate_query_keywords("realworld", "VU", "keywords_list.txt", 5, 10)

    generate_dataset("realworld", "WU", "out.munmun_twitterex_ut", "lognorm", 500, 3)
    generate_query_keywords("realworld", "WU", "keywords_list.txt", 5, 10)

    generate_dataset(os.path.join("synthetic", "dB"), "dBkL", "out.bd-25k", "lognorm", 500, 3)
    generate_query_keywords(os.path.join("synthetic", "dB"), "dBkL", "keywords_list.txt", 5, 10)

    generate_dataset(os.path.join("synthetic", "dP"), "dPkL", "out.pl-25k", "lognorm", 500, 3)
    generate_query_keywords(os.path.join("synthetic", "dP"), "dPkL", "keywords_list.txt", 5, 10)

    generate_dataset(os.path.join("synthetic", "dB"), "dBkP", "out.bd-25k", "pareto", 500, 3)
    generate_query_keywords(os.path.join("synthetic", "dB"), "dBkP", "keywords_list.txt", 5, 10)

    generate_dataset(os.path.join("synthetic", "dP"), "dPkP", "out.pl-25k", "pareto", 500, 3)
    generate_query_keywords(os.path.join("synthetic", "dP"), "dPkP", "keywords_list.txt", 5, 10)

    generate_dataset(os.path.join("synthetic", "dB"), "dBkU", "out.bd-25k", "uniform", 500, 3)
    generate_query_keywords(os.path.join("synthetic", "dB"), "dBkU", "keywords_list.txt", 5, 10)

    generate_dataset(os.path.join("synthetic", "dP"), "dPkU", "out.pl-25k", "uniform", 500, 3)
    generate_query_keywords(os.path.join("synthetic", "dP"), "dPkU", "keywords_list.txt", 5, 10)

    pass
