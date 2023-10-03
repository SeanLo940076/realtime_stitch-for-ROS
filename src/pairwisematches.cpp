#include "pairwisematches.h"

PairwiseMatchResults PairwiseMatches(const std::vector<cv::detail::ImageFeatures>& features,
        const std::string& matcher_type, bool try_cuda, double match_conf, int range_width,
        std::string save_graph_to, std::vector<std::string>& img_names, float conf_thresh)
{
    std::vector<cv::detail::MatchesInfo> pairwise_matches;
    cv::Ptr<cv::detail::FeaturesMatcher> matcher;   
    if (matcher_type == "affine")
        matcher = cv::makePtr<cv::detail::AffineBestOf2NearestMatcher>(false, try_cuda, match_conf);
    else if (range_width==-1)
        matcher = cv::makePtr<cv::detail::BestOf2NearestMatcher>(try_cuda, match_conf);
    else
        matcher = cv::makePtr<cv::detail::BestOf2NearestRangeMatcher>(range_width, try_cuda, match_conf);

    (*matcher)(features, pairwise_matches);
    matcher->collectGarbage();

    // Check if we should save matches graph
    // if (save_graph)
    // {
    // std::string save_graph_to = "/home/sean/Desktop/stitching_realtime/Analyze_Data/Save_graph/matches_graph.dot";
    std::cout << "Saving matches graph..." << std::endl;
    std::ofstream f(save_graph_to.c_str());
    f << matchesGraphAsString(img_names, pairwise_matches, conf_thresh);  
    // }
    
    PairwiseMatchResults results;
    results.features = features;  
    results.pairwise_matches = pairwise_matches;

    
    return results;
}
