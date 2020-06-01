library(grpc)
context("A")

# Test 1
test_that("A", {
  
  expect_match(
    spec <- system.file('examples/helloworld.proto', package = 'grpc'), 
    "Testing"
  )
  
})

# Test 2
