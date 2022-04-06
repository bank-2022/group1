const express = require('express');
const router = express.Router();
const cards = require('../models/cards'); //<- cardsin sijasta users? Vai transfers?

router.post('/', 
function(request, response) {
  cards.add(request.body, function(err, dbResult) {
    if (err) {
      response.json(err);
    } else {
      response.json(request.body);
    }
  });
});

router.get('/:id?',
 function(request, response) {
  if (request.params.id) {
    cards.getById(request.params.id, function(err, dbResult) {
      if (err) {
        response.json(err);
      } else {
        response.json(dbResult);
      }
    });
  } else {
    response.json("Invalid ID")
    /* cards.getAll(function(err, dbResult) {
      if (err) {
        response.json(err);
      } else {
        response.json(dbResult);
      }
    }); */
  }
});

router.put('/:id', 
function(request, response) {
  cards.update(request.params.id, request.body, function(err, dbResult) {
    if (err) {
      response.json(err);
    } else {
      response.json(dbResult);
    }
  });
});

router.delete('/:id', 
function(request, response) {
  cards.delete(request.params.id, function(err, dbResult) {
    if (err) {
      response.json(err);
    } else {
      response.json(dbResult);
    }
  });
});


module.exports = router;