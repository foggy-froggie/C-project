#' @useDynLib packup
"_PACKAGE"

#' @title Assign items to participants using a heap-based greedy algorithm
#'
#' @description
#' This function implements a greedy optimization algorithm using a Min-Heap 
#' to fairly distribute shared equipment and supplies among expedition participants. 
#' It accounts for individual participant strengths and varying item durations 
#'
#' @details
#' The algorithm minimizes the maximum accumulated load across all participants 
#' 
#' First, items are sorted in descending order by their duration, and then by 
#' their weight. Next, using a binary Min-Heap, each item is sequentially assigned 
#' to the participant with the current lowest cumulative effort, defined as:
#' \deqn{Effort = \frac{Weight \times Duration}{Strength}}
#' 
#' If two participants have the exact same cumulative effort, the item is 
#' assigned to the stronger participant.
#'
#' @param weights A numeric vector containing the weights of each item (must be > 0).
#' @param durations An integer vector specifying the number of days
#'   each item must be carried (must be > 0). Continuous equipment should have 
#'   the maximum duration of the trip.
#' @param strength A numeric vector representing the strength/capacity coefficient 
#'   of each participant (must be > 0). Higher values signify higher capacity.
#'
#' @return An integer vector of the same length as \code{weights}. 
#'   Each element represents the index of the participant assigned to that item.
#'
#' @examples
#' # A 3-day expedition with 3 participants of varying strengths
#' item_weights   <- c(5.0, 2.0, 3.5, 1.5, 4.0) # in kg
#' item_durations <- c(3,   1,   2,   3,   3)   # item duration
#' member_strength <- c(1.2, 1.0, 0.8)          # e.g., Guide, You, Beginner
#'
#' assignment <- assign_items(item_weights, item_durations, member_strength)
#' 
#' @export
assign_items <- function(weights, durations, strength) {
  if (!is.numeric(weights)) stop("weights must be numeric")
  if (!is.numeric(strength)) stop("strength must be numeric")
  
  .Call("assign_items", 
        as.double(weights), 
        as.integer(durations), 
        as.double(strength), 
        PACKAGE = "packup")
}