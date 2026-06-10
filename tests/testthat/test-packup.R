library(testthat)

test_that("works for small datasets", {
  weights <- c(10, 20)
  durations <- as.integer(c(1, 1))
  strengths <- c(1, 1)
  
  # Oczekujemy, że każdy dostanie po jednym przedmiocie
  results <- assign_items(weights, durations, strengths)
  
  expect_length(results, 2)
  expect_true(all(results %in% c(1, 2)))
})

test_that("assign_items wyrzuca błędy przy złych danych wejściowych", {
  # Sprawdzenie czy wykrywa ujemne wagi
  expect_error(assign_items(c(-1), as.integer(c(1)), c(1)))
  
  # Sprawdzenie czy wykrywa różną długość wektorów
  expect_error(assign_items(c(1, 2), as.integer(c(1)), c(1)))
  
  # Sprawdzenie czy wykrywa brak osób (wytrzymałość <= 0)
  expect_error(assign_items(c(1), as.integer(c(1)), c(0)))
})

test_that("assign_items obsługuje poprawnie różne wytrzymałości", {
  # Osoba o dużej wytrzymałości powinna teoretycznie dostać cięższy przedmiot
  weights <- c(1, 100)
  durations <- as.integer(c(1, 1))
  strengths <- c(1, 2)
  
  results <- assign_items(weights, durations, strengths)
  
  expect_equal(results[2], 2) 
})