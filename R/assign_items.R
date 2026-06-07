#' @useDynLib packup
"_PACKAGE"

#' @title Assign tasks using heap-based greedy algorithm
#'
#' @param weights Numeric vector of item weights.
#' @param durations Integer vector of item durations.
#' @param strength Numeric vector of person strengths.
#' @return An integer vector where the index is an item id and value is the persons id assigned to that item.
#' @export
assign_items <- function(weights, durations, strength) {
  .Call("assign_items", weights, as.integer(durations), strength, PACKAGE = "packup")
}