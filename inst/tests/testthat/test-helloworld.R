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
while (TRUE) {
    Sys.sleep(1)
    if (any(p$read_output_lines() == "Entering Event Loop")) {
        break()
    }
}

test_that('server running', {
    expect_true(p$is_alive())
})

test_that('using client', {
    expect_true(any(
        grepl('Hello, Neal',
              capture.output(demo('helloclient', 'grpc', ask = FALSE)))))
})

## kill server
p$kill()
