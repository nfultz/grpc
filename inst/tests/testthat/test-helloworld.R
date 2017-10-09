library(testthat)
library(methods)
library(grpc)
library(processx)

context('helloworld example')

## start a server in the background
p <- process$new(commandline = "R -e \"demo('helloserver', 'grpc', ask = FALSE)\"", stdout = "|", stderr = "|")

test_that('server started', {
    expect_true(p$is_alive())
})

## wait for the server to start
for (i in 1:11) {
    Sys.sleep(1)
    if (any(grepl('RUNNING', p$read_output_lines(), fixed = TRUE))) {
        break()
    }
}

test_that('server running', {
    expect_lte(i, 10)
    expect_true(p$is_alive())
})

test_that('using client', {
  expect_output_file(demo('helloclient', 'grpc', ask = FALSE), 'helloworld.out', update = TRUE)
})

## kill server
p$kill()
